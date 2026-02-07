/**
 * OpaqueSheep OS - Kernel Entry Point
 *
 * This file contains the kstup() and kmain() functions.
 * kstup() is called once at boot time for initialization.
 * kmain() is the main execution loop of the OS.
 */

#include "sys.h"

/**
 * Kernel Startup - Runs once after bootloader transitions to kernel
 */
void kstup(void) {
    stup_init(); // Initialize startup system
    fat_init();  // Initialize filesystem
}

/**
 * Kernel Main - Your OS logic goes here
 */
void kmain(void) {
    // Welcome message using system services
    print(svc_get_os_name());
    print(" v");
    print(svc_get_os_version());
    print(" - Template Kernel Loaded.\n");
    print("Type 'h' for help.\n\n");

    // Main shell loop template
    for (;;) {
        const char *cmd = input("> ");
        if (!cmd || cmd[0] == '\0') {
            continue;
        }

        // Command handling template
        if (cmd[0] == 'h') {
            print("h - show this help\n");
            print("i - show system info\n");
            print("p - trigger kernel panic\n");
        } else if (cmd[0] == 'i') {
            print("OS Name: ");
            print(svc_get_os_name());
            print("\nVersion: ");
            print(svc_get_os_version());
            print("\n");
        } else if (cmd[0] == 'p') {
            panic("User-triggered kernel panic.");
        } else {
            print("Unknown command: ");
            print(cmd);
            print("\n");
        }
    }
}

/**
 * Kernel Panic - Called on unrecoverable errors
 */
void panic(const char *message) {
    print("\n!!! KERNEL PANIC !!!\n");
    print(message);
    print("\nSystem Halted.\n");

    for (;;) {
        __asm__ volatile ("hlt");
    }
}
