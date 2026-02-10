#include "services.h"
#include "font.h"
#include "bootparams.h"

// State of the console
static uint32_t* fb_ptr;
static uint32_t screen_width;
static uint32_t screen_height;
static uint32_t ppsl; // Pixels Per Scan Line

static int cursor_x = 0;
static int cursor_y = 0;
static uint32_t text_color = 0xFFFFFFFF; // White
static uint32_t bg_color   = 0x00000000; // Black

void video_init(BootParams* params) {
    fb_ptr = params->fb_base;
    screen_width = params->width;
    screen_height = params->height;
    ppsl = params->ppsl;

    // Clear screen to black initially
    for (uint32_t i = 0; i < ppsl * screen_height; i++) {
        fb_ptr[i] = bg_color;
    }
}

void draw_pixel(int x, int y, uint32_t color) {
    if (x >= 0 && (uint32_t)x < screen_width && y >= 0 && (uint32_t)y < screen_height) {
        fb_ptr[y * ppsl + x] = color;
    }
}

void putc_real(char c) {
    // Handle Newline
    if (c == '\n') {
        cursor_x = 0;
        cursor_y += 16;
    } else if (c == '\r') {
        cursor_x = 0;
    } else {
        // Draw character from font_bitmap
        // Draw character from font_bitmap
        unsigned char uc = (unsigned char)c;
        unsigned char* glyph = &font_bitmap[uc * 16];

        for (int row = 0; row < 16; row++) {
            for (int col = 0; col < 8; col++) {
                // Check if the bit at (col) in glyph[row] is set
                if (glyph[row] & (0x80 >> col)) {
                    draw_pixel(cursor_x + col, cursor_y + row, text_color);
                } else {
                    draw_pixel(cursor_x + col, cursor_y + row, bg_color);
                }
            }
        }
        cursor_x += 8;
    }

    // Wrap text if it hits the edge
    if (cursor_x + 8 > (int)screen_width) {
        cursor_x = 0;
        cursor_y += 16;
    }

    // Scroll if it hits the bottom (Simplified: just reset to top for now)
    if (cursor_y + 16 > (int)screen_height) {
        cursor_y = 0;
    }
}

// The IO Service implementation
void video_write_service(const char* message) {
    while (*message) {
        putc_real(*message++);
    }
}

// Registering the service
static service_t video_svc = {
    .type = SERVICE_IO,
    .name = "GOP_FRAMEBUFFER",
    .init = NULL,
    .write = video_write_service
};

void video_register_service(void) {
    service_register(&video_svc);
}
