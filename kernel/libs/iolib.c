#include "iolib.h"

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

#define COM1 0x3F8

static void serial_init(void) {
    outb(COM1 + 1, 0x00);    // Disable all interrupts
    outb(COM1 + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    outb(COM1 + 0, 0x03);    // Set divisor to 3 (38400 baud)
    outb(COM1 + 1, 0x00);    //                  (hi byte)
    outb(COM1 + 3, 0x03);    // 8 bits, no parity, one stop bit
    outb(COM1 + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    outb(COM1 + 4, 0x0B);    // IRQs enabled, RTS/DSR set
}

static int is_transmit_empty(void) {
    return inb(COM1 + 5) & 0x20;
}

static void serial_putc(char c) {
    while (is_transmit_empty() == 0);
    outb(COM1, c);
}

static volatile uint16_t *const kVgaBuffer = (uint16_t *)0xB8000;
static const uint8_t kDefaultColor = 0x0F;
static const size_t kVgaWidth = 80;
static const size_t kVgaHeight = 25;
static size_t cursor_row = 0;
static size_t cursor_col = 0;

static void iolib_putc(char c) {
    static int initialized = 0;
    if (!initialized) {
        serial_init();
        initialized = 1;
    }

    serial_putc(c);

    if (cursor_row >= kVgaHeight) {
        return;
    }

    if (c == '\n') {
        cursor_col = 0;
        if (cursor_row + 1 < kVgaHeight) {
            cursor_row++;
        }
        return;
    }

    if (cursor_col >= kVgaWidth) {
        cursor_col = 0;
        if (cursor_row + 1 < kVgaHeight) {
            cursor_row++;
        } else {
            return;
        }
    }

    const size_t index = cursor_row * kVgaWidth + cursor_col;
    kVgaBuffer[index] = (uint16_t)kDefaultColor << 8 | (uint8_t)c;
    cursor_col++;
    if (cursor_col >= kVgaWidth) {
        cursor_col = 0;
        if (cursor_row + 1 < kVgaHeight) {
            cursor_row++;
        }
    }
}

void print(const char *message) {
    if (!message) {
        return;
    }

    for (size_t i = 0; message[i] != '\0'; i++) {
        iolib_putc(message[i]);
    }
}
