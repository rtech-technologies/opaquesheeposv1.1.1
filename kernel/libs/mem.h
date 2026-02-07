#ifndef OPAQUESHEEP_MEM_H
#define OPAQUESHEEP_MEM_H

#include "sys.h"

void *memcpy_simple(void *dest, const void *src, size_t count);
void *memset_simple(void *dest, int value, size_t count);

#endif
