#include "services.h"
#include "sys.h"

typedef struct {
    uint32_t cluster_size;
    uint32_t fat_start;
    uint32_t data_start;
    // ... other FAT32 metadata
} fat32_info_t;

size_t real_fwrite(const char* path, const void* data, size_t size, size_t count) {
    // 1. Find file in directory entry
    // 2. Map filename to Cluster Chain
    // 3. Issue Disk Write Command (e.g., AHCI or IDE)
    // 4. Update FAT Table
    (void)path; (void)data; (void)size;
    return count;
}

size_t real_fread(const char* path, void* data, size_t size, size_t count) {
    // Implementation for reading
    (void)path; (void)data; (void)size;
    return count;
}

// Registering it as a service
static service_t fat_svc = {
    .type = SERVICE_FILESYS,
    .name = "FAT32_DISK",
    .write_file = real_fwrite,
    .read_file = real_fread
};

void fat32_init(void) {
    service_register(&fat_svc);
}
