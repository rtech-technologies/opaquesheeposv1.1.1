#ifndef OPAQUESHEEP_SYS_H
#define OPAQUESHEEP_SYS_H

#include <stddef.h>
#include <stdint.h>
#include "bootinfo.h"

void print(const char *message);
const char *input(const char *prompt);
size_t fwrite(const void *data, size_t size, size_t count);
size_t fappend(const void *data, size_t size, size_t count);
size_t fread(void *data, size_t size, size_t count);
int fdelete(const char *path);

int dmake(const char *path);
int dremove(const char *path);
int dmove(const char *from, const char *to);
int fFormat(const char *label);

void *memcpy_simple(void *dest, const void *src, size_t count);
void *memset_simple(void *dest, int value, size_t count);

void fat_init(void);
void stup_init(void);
void iolib_init(boot_info_t *binfo);
void iolib_clear(uint32_t color);
void iolib_reset_cursor(void);

const char *svc_get_os_name(void);
const char *svc_get_os_version(void);
uint64_t svc_get_uptime(void);
size_t svc_get_memory_usage(void);

void kstup(boot_info_t *binfo);
void kmain(void);
void panic(const char *message);

#endif
