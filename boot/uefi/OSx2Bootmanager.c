#include <efi.h>
#include <efilib.h>
#include "../../kernel/include/bootinfo.h"

#define KERNEL_PATH L"\\kernel.bin"

static boot_info_t binfo;

EFI_STATUS EFIAPI efi_main(EFI_HANDLE image, EFI_SYSTEM_TABLE *system_table) {
    ST = system_table;
    BS = system_table->BootServices;

    EFI_STATUS status;
    EFI_LOADED_IMAGE *loaded_image = NULL;
    EFI_DEVICE_PATH *kernel_dp = NULL;
    EFI_HANDLE kernel_handle = NULL;
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop = NULL;

    // Zero binfo
    UINT8 *p = (UINT8 *)&binfo;
    for (UINTN i = 0; i < sizeof(binfo); i++) p[i] = 0;

    // Get Graphics Info
    status = uefi_call_wrapper(BS->LocateProtocol, 3, &gEfiGraphicsOutputProtocolGuid, NULL, (void **)&gop);
    if (!EFI_ERROR(status) && gop) {
        binfo.framebuffer_base = gop->Mode->FrameBufferBase;
        binfo.framebuffer_size = gop->Mode->FrameBufferSize;
        binfo.horizontal_resolution = gop->Mode->Info->HorizontalResolution;
        binfo.vertical_resolution = gop->Mode->Info->VerticalResolution;
        binfo.pixels_per_scanline = gop->Mode->Info->PixelsPerScanLine;
    }

    // Get DevicePath of current image to construct path to kernel
    status = uefi_call_wrapper(BS->HandleProtocol, 3, image, &gEfiLoadedImageProtocolGuid, (void **)&loaded_image);
    if (EFI_ERROR(status)) return status;

    kernel_dp = FileDevicePath(loaded_image->DeviceHandle, KERNEL_PATH);
    if (!kernel_dp) return EFI_OUT_OF_RESOURCES;

    // Load Kernel (PE/COFF)
    status = uefi_call_wrapper(BS->LoadImage, 6, FALSE, image, kernel_dp, NULL, 0, &kernel_handle);
    if (EFI_ERROR(status)) return status;

    // Get Kernel Entry Point
    EFI_LOADED_IMAGE *kernel_loaded = NULL;
    status = uefi_call_wrapper(BS->HandleProtocol, 3, kernel_handle, &gEfiLoadedImageProtocolGuid, (void **)&kernel_loaded);
    if (EFI_ERROR(status)) return status;

    // PE Header parsing for entry point (firmware already loaded and relocated it)
    UINT8 *base = (UINT8 *)kernel_loaded->ImageBase;
    UINT32 pe_offset = *(UINT32 *)(base + 0x3C);
    UINT32 entry_offset = *(UINT32 *)(base + pe_offset + 0x28);
    void *entry = (void *)(base + entry_offset);

    // Exit Boot Services
    UINTN map_key = 0;
    UINTN map_size = 0;
    UINTN desc_size = 0;
    UINT32 desc_version = 0;
    EFI_MEMORY_DESCRIPTOR *memory_map = NULL;

    uefi_call_wrapper(BS->GetMemoryMap, 5, &map_size, NULL, &map_key, &desc_size, &desc_version);
    map_size += 4096;
    status = uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, map_size, (void **)&memory_map);
    if (EFI_ERROR(status)) return status;

    for (int retry = 0; retry < 5; retry++) {
        status = uefi_call_wrapper(BS->GetMemoryMap, 5, &map_size, memory_map, &map_key, &desc_size, &desc_version);
        if (EFI_ERROR(status)) break;

        status = uefi_call_wrapper(BS->ExitBootServices, 2, image, map_key);
        if (!EFI_ERROR(status)) break;
    }

    if (EFI_ERROR(status)) return status;

    // Find the data partition offset (simplified: we know it is partition 2)
    binfo.data_partition_lba = 133120;

    // Transition to Kernel (UEFI x64 ABI: RCX = first argument)
    __asm__ volatile (
        "mov %0, %%rcx\n\t"
        "jmp *%1"
        : : "r"(&binfo), "r"(entry) : "rcx"
    );

    return EFI_SUCCESS;
}
