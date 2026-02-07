#include "iolib.h"

static volatile uint16_t *const kVgaBuffer = (uint16_t *)0xB8000;
static const uint8_t kDefaultColor = 0x0F;
static const size_t kVgaWidth = 80;
static const size_t kVgaHeight = 25;
static size_t cursor_row = 0;
static size_t cursor_col = 0;

static void iolib_putc(char c) {
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
