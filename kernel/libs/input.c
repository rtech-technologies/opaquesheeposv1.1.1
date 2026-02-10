#include "services.h"
#include "io.h"

const char* keyboard_read_string(void) {
    // This is a simplified version. A real one would wait for Enter.
    static char buf[2] = {0, 0};

    // Simple polling for a key (not ideal but works for now)
    while (1) {
        if (inb(0x64) & 1) {
            uint8_t scancode = inb(0x60);
            if (!(scancode & 0x80)) { // Key press
                // Very basic mapping for demonstration
                if (scancode == 0x1E) buf[0] = 'a';
                else if (scancode == 0x30) buf[0] = 'b';
                // ... add more ...
                else buf[0] = '?';
                return buf;
            }
        }
    }
}

static service_t input_svc = {
    .type = SERVICE_INPUT,
    .name = "PS2_KEYBOARD",
    .init = NULL,
    .read_string = keyboard_read_string
};

void input_register_service(void) {
    service_register(&input_svc);
}
