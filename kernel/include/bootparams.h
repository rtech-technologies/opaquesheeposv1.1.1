#ifndef BOOTPARAMS_H
#define BOOTPARAMS_H

#include <stdint.h>

typedef struct {
    uint32_t* fb_base;
    uint32_t  width;
    uint32_t  height;
    uint32_t  ppsl;       // Pixels Per Scan Line
    void*     rdsp;       // ACPI pointer
    uint8_t*  fat_memory; // Map the FAT32 root into memory for the kernel
} BootParams;

#endif
