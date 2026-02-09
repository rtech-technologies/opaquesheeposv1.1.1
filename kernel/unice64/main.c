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
        if (line[0] == '\0') continue;

        char *args = str_split_once(line, ' ');
        const char *cmd = line;

        if (streq(cmd, "help")) {
            print("help   - list commands\n");
            print("info   - system status\n");
            print("ls     - list files\n");
            print("cat    - read file content\n");
            print("touch  - create empty file\n");
            print("rm     - delete file\n");
            print("echo   - write text to file\n");
            print("uptime - show system uptime\n");
            print("format - format virtual disk\n");
            print("panic  - test fatal error\n");
            print("clear  - clear screen\n");
        }

        else if (streq(cmd, "info")) {
            print("OS: "); print(svc_get_os_name()); print("\n");
            print("Version: "); print(svc_get_os_version()); print("\n");
            print("Memory Used: ");
            if (svc_get_memory_usage() > 0) print("4096 KB\n");
        }

        else if (streq(cmd, "ls")) {
            flist();
        }

        else if (streq(cmd, "cat")) {
            if (!args) {
                print("Usage: cat <path>\n");
            } else {
                char buf[512];
                memset_simple(buf, 0, 512);
                size_t r = fread(args, buf, 1, 511);
                if (r > 0) {
                    print(buf);
                    print("\n");
                } else {
                    print("File not found or empty.\n");
                }
            }
        }

        else if (streq(cmd, "touch")) {
            if (!args) {
                print("Usage: touch <path>\n");
            } else {
                fwrite(args, "", 1, 0);
                print("File touched.\n");
            }
        }

        else if (streq(cmd, "rm")) {
            if (!args) {
                print("Usage: rm <path>\n");
            } else {
                if (fdelete(args) == 0) print("File deleted.\n");
                else print("Error deleting file.\n");
            }
        }

        else if (streq(cmd, "echo")) {
            if (!args) {
                print("Usage: echo <path> <text>\n");
            } else {
                char *text = str_split_once(args, ' ');
                if (!text) {
                    print("Usage: echo <path> <text>\n");
                } else {
                    fwrite(args, text, 1, strlen(text));
                    print("Written to "); print(args); print("\n");
                }
            }
        }

        else if (streq(cmd, "uptime")) {
            print("Uptime: ");
            if (svc_get_uptime() > 0) print("Active\n");
            else print("0 ticks\n");
        }

        else if (streq(cmd, "format")) {
            fFormat("OPAQUESHEEP");
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
