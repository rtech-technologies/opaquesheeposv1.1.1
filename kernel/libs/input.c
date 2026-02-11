#include "services.h"
#include "sys.h"

// 8259 PIC Ports
#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1

void pic_init(void) {
    outb(PIC1_COMMAND, 0x11);
    outb(PIC2_COMMAND, 0x11);
    outb(PIC1_DATA, 0x20);
    outb(PIC2_DATA, 0x28);
    outb(PIC1_DATA, 0x04);
    outb(PIC2_DATA, 0x02);
    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);
    outb(PIC1_DATA, 0xFD);
    outb(PIC2_DATA, 0xFF);
}

static char input_buffer[256];
static int buffer_idx = 0;

const char* keyboard_read_string(void) {
    buffer_idx = 0;
    while (1) {
        if (inb(0x64) & 0x01) {
            uint8_t scancode = inb(0x60);
            if (!(scancode & 0x80)) { // Key press
                if (scancode == 0x1C) { // Enter
                    input_buffer[buffer_idx] = '\0';
                    print("\n");
                    return input_buffer;
                } else if (scancode == 0x0E) { // Backspace
                    if (buffer_idx > 0) {
                        buffer_idx--;
                        print("\b \b");
                    }
                } else {
                    // Very basic mapping
                    char c = 0;
                    if (scancode == 0x1E) c = 'a';
                    else if (scancode == 0x30) c = 'b';
                    else if (scancode == 0x2E) c = 'c';
                    else if (scancode == 0x20) c = 'd';
                    else if (scancode == 0x12) c = 'e';
                    else if (scancode == 0x21) c = 'f';
                    else if (scancode == 0x22) c = 'g';
                    else if (scancode == 0x23) c = 'h';
                    else if (scancode == 0x17) c = 'i';
                    else if (scancode == 0x24) c = 'j';
                    else if (scancode == 0x25) c = 'k';
                    else if (scancode == 0x26) c = 'l';
                    else if (scancode == 0x32) c = 'm';
                    else if (scancode == 0x31) c = 'n';
                    else if (scancode == 0x18) c = 'o';
                    else if (scancode == 0x19) c = 'p';
                    else if (scancode == 0x10) c = 'q';
                    else if (scancode == 0x13) c = 'r';
                    else if (scancode == 0x1F) c = 's';
                    else if (scancode == 0x14) c = 't';
                    else if (scancode == 0x16) c = 'u';
                    else if (scancode == 0x2F) c = 'v';
                    else if (scancode == 0x11) c = 'w';
                    else if (scancode == 0x2D) c = 'x';
                    else if (scancode == 0x15) c = 'y';
                    else if (scancode == 0x2C) c = 'z';
                    else if (scancode == 0x39) c = ' ';

                    if (c && buffer_idx < 255) {
                        input_buffer[buffer_idx++] = c;
                        char out[2] = {c, 0};
                        print(out);
                    }
                }
            }
        }
    }
}

static service_t input_svc = {
    .type = SERVICE_INPUT,
    .name = "PS2_KEYBOARD",
    .init = pic_init,
    .read_string = keyboard_read_string
};

void input_register_service(void) {
    service_register(&input_svc);
}
