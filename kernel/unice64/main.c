#include "sys.h"

void kstup(void) {
}

void kmain(void) {
}

void panic(const char *message) {
    (void)message;
    for (;;) {
        __asm__ volatile ("hlt");
    }
}
