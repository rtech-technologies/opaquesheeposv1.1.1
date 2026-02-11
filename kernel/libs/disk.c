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

void ata_read_sector(uint32_t lba, uint16_t* buffer) {
    outb(ATA_PRIMARY_DRIVE_SEL, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_PRIMARY_SECCOUNT, 1);
    outb(ATA_PRIMARY_LBA_LOW,  (uint8_t)lba);
    outb(ATA_PRIMARY_LBA_MID,  (uint8_t)(lba >> 8));
    outb(ATA_PRIMARY_LBA_HIGH, (uint8_t)(lba >> 16));
    outb(ATA_PRIMARY_COMMAND, ATA_CMD_READ_PIO);
    while ((inb(ATA_PRIMARY_STATUS) & ATA_STATUS_BSY));
    while (!(inb(ATA_PRIMARY_STATUS) & ATA_STATUS_DRDY));
    for (int i = 0; i < 256; i++) {
        buffer[i] = inw(ATA_PRIMARY_DATA);
    }
}

void ata_write_sector(uint32_t lba, uint16_t* buffer) {
    outb(ATA_PRIMARY_DRIVE_SEL, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_PRIMARY_SECCOUNT, 1);
    outb(ATA_PRIMARY_LBA_LOW,  (uint8_t)lba);
    outb(ATA_PRIMARY_LBA_MID,  (uint8_t)(lba >> 8));
    outb(ATA_PRIMARY_LBA_HIGH, (uint8_t)(lba >> 16));
    outb(ATA_PRIMARY_COMMAND, ATA_CMD_WRITE_PIO);
    while ((inb(ATA_PRIMARY_STATUS) & ATA_STATUS_BSY));
    while (!(inb(ATA_PRIMARY_STATUS) & ATA_STATUS_DRDY));
    for (int i = 0; i < 256; i++) {
        outw(ATA_PRIMARY_DATA, buffer[i]);
    }
    outb(ATA_PRIMARY_COMMAND, 0xE7); // Cache Flush
    while ((inb(ATA_PRIMARY_STATUS) & ATA_STATUS_BSY));
}
