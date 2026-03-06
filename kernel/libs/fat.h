#ifndef OPAQUESHEEP_FAT_H
#define OPAQUESHEEP_FAT_H

#include "sys.h"

void fat_init(void);
void fat_init_with_info(boot_info_t *binfo);
size_t rnafs_read_file(const char *path, void *buffer, size_t max_size);

#endif
