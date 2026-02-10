/**
 * OpaqueSheep OS - Kernel Entry Point
 *
 * This file contains the kstup() and kmain() functions.
 * kstup() is called once at boot time for initialization.
 * kmain() is the main execution loop of the OS.
 */

#include "sys.h"

/**
 * kstup - OS Startup Orchestrator
 * This function is called by the entry point. It receives the hardware boot info
 * and is responsible for initializing all system backend services.
 */
void kstup(boot_info_t *binfo) {
    iolib_init(binfo);  // Setup modern graphics and serial output
    stup_init();        // Run core startup sequence
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
    print(svc_get_os_name());
    print(" v");
    print(svc_get_os_version());
    print(" - Professional Shell Loaded.\n");
    print("Type 'help' for commands.\n\n");

    for (;;) {
        char *line = (char *)input("> ");
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
