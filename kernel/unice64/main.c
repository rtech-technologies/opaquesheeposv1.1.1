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
        const char *cmd = input("> ");

        if (streq(cmd, "help")) {
            print("help   - list commands\n");
            print("info   - system status\n");
            print("format - format virtual disk\n");
            print("write  - write test data\n");
            print("read   - read test data\n");
            print("panic  - test fatal error\n");
            print("clear  - clear screen\n");
        }

        else if (streq(cmd, "info")) {
            print("OS: "); print(svc_get_os_name()); print("\n");
            print("Version: "); print(svc_get_os_version()); print("\n");
            print("Memory Used: ");
            if (svc_get_memory_usage() > 0) print("4096 KB\n");
        }

        else if (streq(cmd, "format")) {
            fFormat("OPAQUESHEEP");
        }

        else if (streq(cmd, "write")) {
            fwrite("Hello OpaqueSheep!", 1, 18);
            print("Data written to disk.\n");
        }

        else if (streq(cmd, "read")) {
            char buf[32];
            memset_simple(buf, 0, 32);
            fread(buf, 1, 18);
            print("Disk Contents: "); print(buf); print("\n");
        }

        else if (streq(cmd, "panic")) {
            panic("User requested fatal error.");
        }

        else if (streq(cmd, "clear")) {
            iolib_clear(0x000000);
            iolib_reset_cursor();
        }

        else if (cmd[0] != '\0') {
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
    iolib_clear(0xFF0000); // Red Background
    iolib_reset_cursor();
    print("!!! KERNEL PANIC !!!\n\n");
    print(message);
    print("\n\nSystem Halted.\n");

    for (;;) {
        __asm__ volatile ("hlt");
    }
}
