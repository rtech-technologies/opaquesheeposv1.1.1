#include "sys.h"
#include "fat.h"
#include "iolib.h"

#include "iolib.h"

static uint64_t data_lba = 0;

void fat_init_with_info(boot_info_t *binfo) {
    if (binfo) {
        data_lba = binfo->data_partition_lba;
    }
}

/**
 * rnafs_read_file - Simplified Optimized FAT reader for RSL files
 * RNAFS is FAT-based but optimized for OpaqueSheep OS.
 */
size_t rnafs_read_file(const char *path, void *buffer, size_t max_size) {
    if (!path || !buffer || data_lba == 0) return 0;

    /* RNAFS Mock: In a real implementation, this would perform ATA PIO
       to read sectors starting at data_lba. For now, we simulate the
       success of reading a known system script if requested. */

    if (streq(path, "init.rsl")) {
        const char *script = "echo RNAFS Initialized\nls\nuptime\n";
        size_t len = strlen(script);
        if (len > max_size) len = max_size;
        memcpy_simple(buffer, script, len);
        return len;
    }

    return 0;
}

void fat_init(void) {
    print("RNAFS: System partition linked.\n");
}
