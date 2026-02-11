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

// Hardware I/O
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outb(uint16_t port, uint8_t val) {
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint16_t inw(uint16_t port) {
    uint16_t ret;
    asm volatile ("inw %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outw(uint16_t port, uint16_t val) {
    asm volatile ("outw %0, %1" : : "a"(val), "Nd"(port));
}

#endif
