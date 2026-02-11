#include "sys.h"

struct idt_entry {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t  ist;
    uint8_t  types_attr;
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t zero;
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

static struct idt_entry idt[256];
static struct idt_ptr idtp;

void idt_set_gate(uint8_t num, uint64_t base, uint16_t sel, uint8_t flags) {
    idt[num].offset_low = (base & 0xFFFF);
    idt[num].offset_mid = (base >> 16) & 0xFFFF;
    idt[num].offset_high = (base >> 32) & 0xFFFFFFFF;
    idt[num].selector = sel;
    idt[num].ist = 0;
    idt[num].types_attr = flags;
    idt[num].zero = 0;
}

extern void isr1(); // Defined in assembly

void idt_init() {
    idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
    idtp.base = (uint64_t)&idt;

    // Set IRQ1 (Keyboard) to entry 0x21 (remapped by PIC)
    idt_set_gate(0x21, (uint64_t)isr1, 0x08, 0x8E);

    asm volatile ("lidt %0" : : "m"(idtp));
}

void keyboard_handler_c() {
    uint8_t scancode = inb(0x60);
    // In a real ISR, we would push to a buffer.
    // For now, we just acknowledge the interrupt.
    outb(0x20, 0x20); // Send EOI
}
