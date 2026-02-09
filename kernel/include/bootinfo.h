#ifndef OPAQUESHEEP_BOOTINFO_H
#define OPAQUESHEEP_BOOTINFO_H

#include "rtech.h"

typedef struct {
    uint64_t framebuffer_base;
    uint64_t framebuffer_size;
    uint32_t horizontal_resolution;
    uint32_t vertical_resolution;
    uint32_t pixels_per_scanline;
} boot_info_t;

#endif
