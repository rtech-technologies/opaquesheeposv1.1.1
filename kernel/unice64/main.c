/**
 * OpaqueSheep OS - Kernel Entry Point
 *
 * This file contains the STup() and kmain() functions.
 * STup() is called once at boot time for initialization.
 * kmain() is the main execution loop of the OS.
 */

#include "sys.h"

/**
 * STup - OS Startup Orchestrator
 * This function is called by the entry point. It receives the hardware boot info
 * and is responsible for initializing all system backend services.
 */
void STup(boot_info_t *binfo) {
    iolib_init(binfo);  // Setup modern graphics and serial output
    stup_init();        // Run core startup sequence
    fat_init_with_info(binfo);
    fat_init();         // Initialize filesystem drivers
    print("System Initialized.\n");
}


/**
 * kmain - Professional CLI Shell (High-Level API Only)
 * Write your behavior here like a high-level script.
 * Use the System API (sys.h) for all logic.
 */
void kmain(boot_info_t *binfo) {
    (void)binfo;
    print("OpaqueSheep OS Booted (UEFI 3.0 Compliance Mode)\n");
    print("System Partition: GPT ESP Mounted\n");
    print("Data Partition: GPT Main Mounted\n\n");

    print("Welcome! Type something to echo it back, or 'shell' for RSL.\n");

    for (;;) {
        const char *line = input("echo> ");
        if (streq(line, "shell")) {
            print("Entering Rtech Script Language (RSL) Shell...\n");
            break;
        }
        if (line[0] != '\0') {
            print("You said: ");
            print(line);
            print("\n");
        }
    }

    print("\nRSL Shell Loaded. Type 'help' for commands.\n\n");
    for (;;) {
        char *line = (char *)input("rsl> ");
        exec_line(line);
    }
}

/**
 * Kernel Panic - Called on unrecoverable errors
 */
void panic(const char *message) {
    iolib_clear(0xFF0000); // Red Background
    iolib_reset_cursor();
    print("!!! KERNEL PANIC !!!\n\n");
    print(message);
    print("\n\nSystem Halted.\n");

    for (;;) {
        __asm__ volatile ("hlt");
    }
}
