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

extern void isr1();  // Keyboard
extern void isr14(); // ATA Primary

void idt_init() {
    idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
    idtp.base = (uint64_t)&idt;

    // IRQ1 (Keyboard) -> 0x21
    idt_set_gate(0x21, (uint64_t)isr1, 0x08, 0x8E);
    // IRQ14 (ATA Primary) -> 0x2E
    idt_set_gate(0x2E, (uint64_t)isr14, 0x08, 0x8E);

    asm volatile ("lidt %0" : : "m"(idtp));
    asm volatile ("sti"); // Enable interrupts
}

void keyboard_handler_c() {
    // Handling scancode reading in input.c via polling for now
    // or we can move it here. For the mandate, we just ensure IRQ is handled.
    (void)inb(0x60);
    outb(0x20, 0x20); // Master EOI
}

extern void ata_irq_handler(void);
void disk_handler_c() {
    ata_irq_handler();
    outb(0xA0, 0x20); // Slave EOI
    outb(0x20, 0x20); // Master EOI
}

void pic_init(void) {
    // ICW1
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    // ICW2
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    // ICW3
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    // ICW4
    outb(0x21, 0x01);
    outb(0xA1, 0x01);

    // Unmask IRQ1 (Keyboard) and IRQ14 (ATA)
    // Keyboard is bit 1 of master, ATA is bit 6 of slave (IRQ14 = 8 + 6)
    outb(0x21, 0xFD); // 1111 1101
    outb(0xA1, 0xBF); // 1011 1111
}
