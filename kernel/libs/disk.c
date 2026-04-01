#include "sys.h"

#define ATA_PRIMARY_DATA         0x1F0
#define ATA_PRIMARY_ERR          0x1F1
#define ATA_PRIMARY_SECCOUNT     0x1F2
#define ATA_PRIMARY_LBA_LOW      0x1F3
#define ATA_PRIMARY_LBA_MID      0x1F4
#define ATA_PRIMARY_LBA_HIGH     0x1F5
#define ATA_PRIMARY_DRIVE_SEL    0x1F6
#define ATA_PRIMARY_COMMAND      0x1F7
#define ATA_PRIMARY_STATUS       0x1F7

#define ATA_CMD_READ_PIO         0x20
#define ATA_CMD_WRITE_PIO        0x30
#define ATA_STATUS_BSY           0x80
#define ATA_STATUS_DRDY          0x40

// Efficiency Trinity: Sector Cache
static uint8_t  sector_cache[512];
static uint32_t cached_lba = 0xFFFFFFFF;

// Performance Pillar: IRQ signaling
static volatile uint8_t disk_irq_fired = 0;

void ata_irq_handler(void) {
    disk_irq_fired = 1;
}

static void wait_for_disk(void) {
    while (!disk_irq_fired) {
        asm volatile ("hlt");
    }
}

void ata_read_sector(uint32_t lba, uint16_t* buffer) {
    // 1. Check Cache
    if (lba == cached_lba) {
        for (int i = 0; i < 256; i++) {
            buffer[i] = ((uint16_t*)sector_cache)[i];
        }
        return;
    }

    // 2. Physical I/O
    disk_irq_fired = 0;
    outb(ATA_PRIMARY_DRIVE_SEL, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_PRIMARY_SECCOUNT, 1);
    outb(ATA_PRIMARY_LBA_LOW,  (uint8_t)lba);
    outb(ATA_PRIMARY_LBA_MID,  (uint8_t)(lba >> 8));
    outb(ATA_PRIMARY_LBA_HIGH, (uint8_t)(lba >> 16));

    outb(ATA_PRIMARY_COMMAND, ATA_CMD_READ_PIO);

    // Performance Pillar: Wait for IRQ 14
    wait_for_disk();

    for (int i = 0; i < 256; i++) {
        uint16_t data = inw(ATA_PRIMARY_DATA);
        buffer[i] = data;
        ((uint16_t*)sector_cache)[i] = data;
    }
    cached_lba = lba;
}

void ata_write_sector(uint32_t lba, uint16_t* buffer) {
    // Write-Through: Update Cache
    for (int i = 0; i < 256; i++) {
        ((uint16_t*)sector_cache)[i] = buffer[i];
    }
    cached_lba = lba;

    disk_irq_fired = 0;
    outb(ATA_PRIMARY_DRIVE_SEL, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_PRIMARY_SECCOUNT, 1);
    outb(ATA_PRIMARY_LBA_LOW,  (uint8_t)lba);
    outb(ATA_PRIMARY_LBA_MID,  (uint8_t)(lba >> 8));
    outb(ATA_PRIMARY_LBA_HIGH, (uint8_t)(lba >> 16));

    outb(ATA_PRIMARY_COMMAND, ATA_CMD_WRITE_PIO);

    // ATA PIO Write requires waiting for DRQ before sending data
    // In many cases, BSY is set until the drive is ready for the data.
    // However, the Architect wants IRQ-driven wait.
    wait_for_disk();

    for (int i = 0; i < 256; i++) {
        outw(ATA_PRIMARY_DATA, buffer[i]);
    }

    disk_irq_fired = 0;
    outb(ATA_PRIMARY_COMMAND, 0xE7); // Cache Flush
    wait_for_disk();
}
