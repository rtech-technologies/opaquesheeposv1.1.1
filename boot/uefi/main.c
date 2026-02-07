#include <efi.h>
#include <efilib.h>

#define KERNEL_PATH L"\\kernel.bin"
#define KERNEL_LOAD_ADDRESS 0x100000

typedef void (*kernel_entry_t)(void);

static EFI_STATUS open_kernel(EFI_HANDLE image, EFI_FILE_PROTOCOL **file) {
    EFI_STATUS status;
    EFI_LOADED_IMAGE *loaded_image = NULL;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *fs = NULL;
    EFI_FILE_PROTOCOL *root = NULL;

    status = uefi_call_wrapper(BS->HandleProtocol, 3, image, &gEfiLoadedImageProtocolGuid, (void **)&loaded_image);
    if (EFI_ERROR(status)) {
        Print(L"HandleProtocol(LoadedImage) failed: %r\n", status);
        return status;
    }

    status = uefi_call_wrapper(BS->HandleProtocol, 3, loaded_image->DeviceHandle,
                               &gEfiSimpleFileSystemProtocolGuid, (void **)&fs);
    if (EFI_ERROR(status)) {
        Print(L"HandleProtocol(SimpleFS) failed: %r\n", status);
        return status;
    }

    status = uefi_call_wrapper(fs->OpenVolume, 2, fs, &root);
    if (EFI_ERROR(status)) {
        Print(L"OpenVolume failed: %r\n", status);
        return status;
    }

    status = uefi_call_wrapper(root->Open, 5, root, file, KERNEL_PATH,
                               EFI_FILE_MODE_READ, 0);
    if (EFI_ERROR(status)) {
        Print(L"Open %s failed: %r\n", KERNEL_PATH, status);
    }
    return status;
}

static EFI_STATUS load_kernel(EFI_FILE_PROTOCOL *file, EFI_PHYSICAL_ADDRESS *entry) {
    EFI_STATUS status;
    EFI_FILE_INFO *info = NULL;
    UINTN info_size = sizeof(EFI_FILE_INFO) + 256;

    status = uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, info_size, (void **)&info);
    if (EFI_ERROR(status)) {
        Print(L"AllocatePool(FileInfo) failed: %r\n", status);
        return status;
    }

    status = uefi_call_wrapper(file->GetInfo, 4, file, &gEfiFileInfoGuid, &info_size, info);
    if (EFI_ERROR(status)) {
        Print(L"GetInfo(FileInfo) failed: %r\n", status);
        uefi_call_wrapper(BS->FreePool, 1, info);
        return status;
    }

    EFI_PHYSICAL_ADDRESS kernel_addr = KERNEL_LOAD_ADDRESS;
    UINTN pages = (info->FileSize + EFI_PAGE_SIZE - 1) / EFI_PAGE_SIZE;

    status = uefi_call_wrapper(BS->AllocatePages, 4, AllocateAddress, EfiLoaderData, pages, &kernel_addr);
    if (EFI_ERROR(status)) {
        Print(L"AllocatePages failed: %r\n", status);
        uefi_call_wrapper(BS->FreePool, 1, info);
        return status;
    }

    UINTN read_size = info->FileSize;
    status = uefi_call_wrapper(file->Read, 3, file, &read_size, (void *)(UINTN)kernel_addr);
    uefi_call_wrapper(BS->FreePool, 1, info);
    if (EFI_ERROR(status)) {
        Print(L"Read kernel failed: %r\n", status);
        return status;
    }

    *entry = kernel_addr;
    return EFI_SUCCESS;
}

EFI_STATUS EFIAPI efi_main(EFI_HANDLE image, EFI_SYSTEM_TABLE *system_table) {
    InitializeLib(image, system_table);

    EFI_FILE_PROTOCOL *kernel = NULL;
    EFI_STATUS status = open_kernel(image, &kernel);
    if (EFI_ERROR(status)) {
        Print(L"Failed to open kernel.bin: %r\n", status);
        return status;
    }

    EFI_PHYSICAL_ADDRESS entry = 0;
    status = load_kernel(kernel, &entry);
    uefi_call_wrapper(kernel->Close, 1, kernel);
    if (EFI_ERROR(status)) {
        Print(L"Failed to load kernel.bin: %r\n", status);
        return status;
    }

    UINTN map_key = 0;
    UINTN map_size = 0;
    UINTN desc_size = 0;
    UINT32 desc_version = 0;
    EFI_MEMORY_DESCRIPTOR *memory_map = NULL;

    status = uefi_call_wrapper(BS->GetMemoryMap, 5, &map_size, memory_map, &map_key, &desc_size, &desc_version);
    if (status == EFI_BUFFER_TOO_SMALL) {
        status = uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, map_size, (void **)&memory_map);
        if (EFI_ERROR(status)) {
            Print(L"AllocatePool(MemoryMap) failed: %r\n", status);
            return status;
        }

        status = uefi_call_wrapper(BS->GetMemoryMap, 5, &map_size, memory_map, &map_key, &desc_size, &desc_version);
    }
    if (EFI_ERROR(status)) {
        Print(L"GetMemoryMap failed: %r\n", status);
        return status;
    }

    if (!EFI_ERROR(status)) {
        status = uefi_call_wrapper(BS->ExitBootServices, 2, image, map_key);
    }

    if (EFI_ERROR(status)) {
        Print(L"ExitBootServices failed: %r\n", status);
        return status;
    }

    kernel_entry_t kernel_entry = (kernel_entry_t)(UINTN)entry;
    kernel_entry();

    return EFI_SUCCESS;
}
