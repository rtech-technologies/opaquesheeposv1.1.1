#include "sys.h"
#include "bootparams.h"

// External service registrations
void video_init(BootParams* params);
void video_register_service(void);
void input_register_service(void);
void fat32_init(void);
void fFormat(const char* label);
void flist(void);

void kstup(BootParams* params) {
    // 1. Initialize hardware services
    video_init(params);
    video_register_service();
    input_register_service();
    fat32_init();

    print("OpaqueSheep OS v2.0 - Real Boy Hardware Layer\n");
    print("Microkernel-lite Service Registry Initialized.\n");
    print("--------------------------------------------------\n");

    while (1) {
        const char* cmd = input("> ");
        if (streq(cmd, "help")) {
            print("Commands: help, format, version, ls\n");
        } else if (streq(cmd, "format")) {
            fFormat("OPAQUESHEEP");
        } else if (streq(cmd, "version")) {
            print("OpaqueSheep OS v2.0 (GPT/FAT32 Mode)\n");
        } else if (streq(cmd, "ls")) {
            flist();
        } else {
            print("Unknown command.\n");
        }
    }
}
