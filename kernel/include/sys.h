#ifndef SYS_H
#define SYS_H

#include <stddef.h>
#include <stdint.h>

// APL Prototypes
void print(const char* message);
const char* input(const char* prompt);
size_t fwrite(const char* path, const void* data, size_t size, size_t count);
size_t fread(const char* path, void* data, size_t size, size_t count);

// Utility
static inline int streq(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 == *(const unsigned char*)s2;
}

#endif
