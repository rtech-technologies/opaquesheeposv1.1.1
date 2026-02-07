#ifndef OPAQUESHEEP_FILELIB_H
#define OPAQUESHEEP_FILELIB_H

#include "sys.h"

size_t fwrite(const void *data, size_t size, size_t count);
size_t fappend(const void *data, size_t size, size_t count);
size_t fread(void *data, size_t size, size_t count);
int fdelete(const char *path);

int dmake(const char *path);
int dremove(const char *path);
int dmove(const char *from, const char *to);
int fFormat(const char *label);

#endif
