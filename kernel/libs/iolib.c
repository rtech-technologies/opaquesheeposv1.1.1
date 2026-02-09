#include "iolib.h"
#include "sys.h"
#include "font.h"

static boot_info_t g_binfo_data;
static boot_info_t *g_binfo = NULL;

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

static int is_data_ready(void) {
    return inb(COM1 + 5) & 0x01;
}

static char serial_getc(void) {
    while (is_data_ready() == 0);
    return inb(COM1);
}

static size_t cursor_row = 0;
static size_t cursor_col = 0;

void iolib_clear(uint32_t color) {
    if (g_binfo && g_binfo->framebuffer_base) {
        uint32_t *fb = (uint32_t *)g_binfo->framebuffer_base;
        for (uint64_t i = 0; i < g_binfo->framebuffer_size / 4; i++) {
            fb[i] = color;
        }
    }
}

void iolib_reset_cursor(void) {
    cursor_row = 0;
    cursor_col = 0;
}

void iolib_init(boot_info_t *binfo) {
    if (binfo) {
        g_binfo_data = *binfo;
        g_binfo = &g_binfo_data;
    }
    iolib_clear(0x000000); // Black
    iolib_reset_cursor();
}

static void draw_pixel(uint32_t x, uint32_t y, uint32_t color) {
    if (!g_binfo || !g_binfo->framebuffer_base) return;
    if (x >= g_binfo->horizontal_resolution || y >= g_binfo->vertical_resolution) return;

    uint32_t *fb = (uint32_t *)g_binfo->framebuffer_base;
    fb[y * g_binfo->pixels_per_scanline + x] = color;
}

static void draw_char(char c, uint32_t x, uint32_t y, uint32_t color) {
    if ((unsigned char)c >= 128) c = 0;
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            if (font8x8[(int)c][row] & (0x80 >> col)) {
                draw_pixel(x + col, y + row, color);
            }
        }
    }
}

static void iolib_putc(char c) {
    static int initialized = 0;
    if (!initialized) {
        serial_init();
        initialized = 1;
    }

    serial_putc(c);

    if (!g_binfo || !g_binfo->framebuffer_base) return;

    uint32_t chars_per_line = g_binfo->horizontal_resolution / 8;
    uint32_t lines = g_binfo->vertical_resolution / 8;

    if (c == '\n') {
        cursor_col = 0;
        cursor_row++;
    } else {
        draw_char(c, cursor_col * 8, cursor_row * 8, 0xFFFFFF); // White
        cursor_col++;
        if (cursor_col >= chars_per_line) {
            cursor_col = 0;
            cursor_row++;
        }
    }

    if (cursor_row >= lines) {
        // Simple scroll: clear and reset for now
        iolib_init(g_binfo);
        cursor_row = 0;
        cursor_col = 0;
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

char iolib_getc(void) {
    static int initialized = 0;
    if (!initialized) {
        serial_init();
        initialized = 1;
    }
    char c = serial_getc();
    if (c == '\r') c = '\n'; // Convert CR to NL
    return c;
}
