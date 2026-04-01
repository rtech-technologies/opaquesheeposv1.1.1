#ifndef FAT32_H
#define FAT32_H

#include <stdint.h>

typedef struct {
    uint8_t  name[11];
    uint8_t  attributes;
    uint8_t  reserved;
    uint8_t  create_time_tenth;
    uint16_t create_time;
    uint16_t create_date;
    uint16_t last_access_date;
    uint16_t cluster_high;
    uint16_t write_time;
    uint16_t write_date;
    uint16_t cluster_low;
    uint32_t file_size;
} __attribute__((packed)) DirectoryEntry;

typedef struct {
    uint32_t lead_sig;          // 0x41615252
    uint8_t  reserved1[480];
    uint32_t struct_sig;        // 0x61417272
    uint32_t free_count;        // Last known free cluster count
    uint32_t next_free;         // Hint for next free cluster
    uint8_t  reserved2[12];
    uint32_t trail_sig;         // 0xAA550000
} __attribute__((packed)) fat32_fsinfo_t;

#endif
