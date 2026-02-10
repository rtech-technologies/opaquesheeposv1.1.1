#include "sys.h"
#include "bootparams.h"

// External service registrations
void video_init(BootParams* params);
void video_register_service(void);
void input_register_service(void);
void fat32_init(void);

void kmain(BootParams* params) {
    // 1. Initialize hardware services
    video_init(params);
    video_register_service();
    input_register_service();
    fat32_init();

    print("OpaqueSheep OS v2.0 - Real Real Real Hardware Layer\n");
    print("Microkernel-lite Service Registry Initialized.\n");
    print("--------------------------------------------------\n");

    while (1) {
        const char* cmd = input("> ");
        if (streq(cmd, "help")) {
            print("Commands: help, clear, version, ls\n");
        } else if (streq(cmd, "version")) {
            print("OpaqueSheep OS v2.0 (UEFI Native)\n");
        } else if (streq(cmd, "ls")) {
            print("FAT32 Root: No files found.\n");
        } else {
            print("Unknown command.\n");
        }
    }
}
