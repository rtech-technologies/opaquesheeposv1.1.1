#include "sys.h"

void kstup(void) {
    stup_init();
    fat_init();
}

void kmain(void) {
    print("OpaqueSheep OS (x86_64) - Professional Shell\n");
    print("Type 'h' for help.\n\n");

    for (;;) {
        const char *cmd = input("> ");
        if (!cmd || cmd[0] == '\0') {
            continue;
        }

        if (cmd[0] == 'h') {
            print("h - help\n");
            print("f - format filesystem\n");
            print("p - panic test\n");
        } else if (cmd[0] == 'f') {
            fFormat("OPAQUESHEEP");
        } else if (cmd[0] == 'p') {
            panic("User requested panic\n");
        } else {
            print("Unknown command\n");
        }
    }
}

void panic(const char *message) {
    (void)message;
    for (;;) {
        __asm__ volatile ("hlt");
    }
}
