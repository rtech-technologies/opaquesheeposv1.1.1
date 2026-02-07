#include "iolib.h"

static volatile uint16_t *const kVgaBuffer = (uint16_t *)0xB8000;
static const uint8_t kDefaultColor = 0x0F;
static size_t cursor_row = 0;
static size_t cursor_col = 0;

static void iolib_putc(char c) {
    if (c == '\n') {
        cursor_col = 0;
        cursor_row++;
        return;
    }

    const size_t index = cursor_row * 80 + cursor_col;
    kVgaBuffer[index] = (uint16_t)kDefaultColor << 8 | (uint8_t)c;
    cursor_col++;
    if (cursor_col >= 80) {
        cursor_col = 0;
        cursor_row++;
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
