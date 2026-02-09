#ifndef OPAQUESHEEP_SYS_H
#define OPAQUESHEEP_SYS_H

/* OpaqueSheep OS - High-Level System API Wrapper for Rtech Standard Library */

#include "rtech.h"
#include "bootinfo.h"

/* Kernel Lifecycle & Initialization */
void kstup(boot_info_t *binfo);
void kmain(boot_info_t *binfo);
void panic(const char *message);

/* Internal System Init (Called by kstup) */
void fat_init(void);
void stup_init(void);
void iolib_init(boot_info_t *binfo);

#endif
