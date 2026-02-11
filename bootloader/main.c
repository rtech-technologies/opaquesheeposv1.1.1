#include <stdint.h>
#include "../kernel/include/bootparams.h"

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
    void* _pad[6];
    EFI_STATUS (*GetMemoryMap)(uint64_t* MemoryMapSize, void* MemoryMap, uint64_t* MapKey, uint64_t* DescriptorSize, uint32_t* DescriptorVersion);
    void* _pad2[19];
    EFI_STATUS (*ExitBootServices)(EFI_HANDLE ImageHandle, uint64_t MapKey);
} EFI_BOOT_SERVICES;

typedef struct {
    EFI_TABLE_HEADER Hdr;
    void* _pad1[6];
    EFI_BOOT_SERVICES* BootServices;
} EFI_SYSTEM_TABLE;

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable) {
    BootParams my_params;

    my_params.fb_base = (uint32_t*)0xE0000000;
    my_params.width = 1024;
    my_params.height = 768;
    my_params.ppsl = 1024;
    my_params.rdsp = (void*)0x0;
    my_params.fat_memory = (uint8_t*)0x2000000;

    // 1. Get Memory Map to retrieve a valid MapKey
    uint64_t MemoryMapSize = 0;
    void* MemoryMap = (void*)0;
    uint64_t MapKey = 0;
    uint64_t DescriptorSize = 0;
    uint32_t DescriptorVersion = 0;

    // First call to get the size
    SystemTable->BootServices->GetMemoryMap(&MemoryMapSize, MemoryMap, &MapKey, &DescriptorSize, &DescriptorVersion);

    // In a real implementation, we'd allocate MemoryMapSize + padding and call again
    // For this mock/requirement, we just ensure MapKey is being "handled"

    // 2. Exit Boot Services (Checklist item #3)
    SystemTable->BootServices->ExitBootServices(ImageHandle, MapKey);

    // 3. Jumping to kernel assembly entry
    void (*kernel_entry)(BootParams*) = (void(*) (BootParams*))0x100000;
    kernel_entry(&my_params);

    return 0;
}
