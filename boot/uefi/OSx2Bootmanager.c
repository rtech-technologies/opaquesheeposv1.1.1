#include <efi.h>
#include <efilib.h>
#include "../../kernel/include/bootinfo.h"

#define KERNEL_PATH L"\\kernel.bin"
#define KERNEL_LOAD_ADDR 0x200000

static boot_info_t binfo;

EFI_STATUS EFIAPI efi_main(EFI_HANDLE image, EFI_SYSTEM_TABLE *system_table) {
    ST = system_table;
    BS = system_table->BootServices;

    EFI_STATUS status;
    EFI_LOADED_IMAGE *loaded_image = NULL;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *fs = NULL;
    EFI_FILE_IO_INTERFACE *io = NULL;
    EFI_FILE_HANDLE root = NULL;
    EFI_FILE_HANDLE kernel_file = NULL;
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop = NULL;

    InitializeLib(image, system_table);

    // Get Graphics Info
    status = uefi_call_wrapper(BS->LocateProtocol, 3, &gEfiGraphicsOutputProtocolGuid, NULL, (void **)&gop);
    if (!EFI_ERROR(status) && gop) {
        binfo.framebuffer_base = gop->Mode->FrameBufferBase;
        binfo.framebuffer_size = gop->Mode->FrameBufferSize;
        binfo.horizontal_resolution = gop->Mode->Info->HorizontalResolution;
        binfo.vertical_resolution = gop->Mode->Info->VerticalResolution;
        binfo.pixels_per_scanline = gop->Mode->Info->PixelsPerScanLine;
    }

    // Load Kernel manually
    status = uefi_call_wrapper(BS->HandleProtocol, 3, image, &gEfiLoadedImageProtocolGuid, (void **)&loaded_image);
    if (EFI_ERROR(status)) return status;

    status = uefi_call_wrapper(BS->HandleProtocol, 3, loaded_image->DeviceHandle, &gEfiSimpleFileSystemProtocolGuid, (void **)&fs);
    if (EFI_ERROR(status)) return status;

    status = uefi_call_wrapper(fs->OpenVolume, 2, fs, &root);
    if (EFI_ERROR(status)) return status;

    status = uefi_call_wrapper(root->Open, 5, root, &kernel_file, KERNEL_PATH, EFI_FILE_MODE_READ, 0);
    if (EFI_ERROR(status)) return status;

    // Allocate memory for kernel at fixed address
    EFI_PHYSICAL_ADDRESS kernel_addr = KERNEL_LOAD_ADDR;
    status = uefi_call_wrapper(BS->AllocatePages, 4, AllocateAddress, EfiLoaderData, 100, &kernel_addr);
    if (EFI_ERROR(status)) return status;

    UINTN kernel_size = 409600; // Max size
    status = uefi_call_wrapper(kernel_file->Read, 3, kernel_file, &kernel_size, (void *)kernel_addr);
    if (EFI_ERROR(status)) return status;

    uefi_call_wrapper(kernel_file->Close, 1, kernel_file);
    uefi_call_wrapper(root->Close, 1, root);

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

    // Set Data partition LBA (from Makefile knowledge)
    binfo.data_partition_lba = 133120;

    // Transition to Kernel (System V ABI bridge expects binfo in RCX)
    typedef void (*kernel_entry_t)(boot_info_t *);
    kernel_entry_t kernel_entry = (kernel_entry_t)kernel_addr;

    __asm__ volatile (
        "mov %0, %%rcx\n\t"
        "jmp *%1"
        : : "r"(&binfo), "r"(kernel_entry) : "rcx"
    );

    return EFI_SUCCESS;
}
