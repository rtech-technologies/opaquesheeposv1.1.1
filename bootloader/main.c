#include <stdint.h>
#include "../kernel/include/bootparams.h"

// Minimal UEFI definitions (x86_64)
typedef void* EFI_HANDLE;
typedef uint64_t EFI_STATUS;

typedef struct {
    uint64_t Signature;
    uint32_t Revision;
    uint32_t HeaderSize;
    uint32_t CRC32;
    uint32_t Reserved;
} EFI_TABLE_HEADER;

typedef struct {
    EFI_TABLE_HEADER Hdr;
    // Function pointers...
    void* _pad[26];
    EFI_STATUS (*ExitBootServices)(EFI_HANDLE ImageHandle, uint64_t MapKey);
} EFI_BOOT_SERVICES;

typedef struct {
    EFI_TABLE_HEADER Hdr;
    void* _pad1[6]; // Skip Console stuff
    EFI_BOOT_SERVICES* BootServices;
} EFI_SYSTEM_TABLE;

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable) {
    BootParams my_params;

    // 1. In a real system, we'd find GOP and fill these:
    my_params.fb_base = (uint32_t*)0xE0000000;
    my_params.width = 1024;
    my_params.height = 768;
    my_params.ppsl = 1024;
    my_params.rdsp = (void*)0x0;
    my_params.fat_memory = (uint8_t*)0x2000000;

    // 2. Call ExitBootServices (Checklist Item #3)
    // In this mock, we don't have a real map_key, but we call the pointer.
    // SystemTable->BootServices->ExitBootServices(ImageHandle, 0);

    // 3. Jump to kernel at 1MB
    void (*kernel_entry)(BootParams*) = (void(*) (BootParams*))0x100000;
    kernel_entry(&my_params);

    return 0;
}
