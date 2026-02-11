#include "services.h"
#include "sys.h"
#include "fat32.h"

// External ATA driver functions
void ata_read_sector(uint32_t lba, uint16_t* buffer);
void ata_write_sector(uint32_t lba, uint16_t* buffer);

typedef struct {
    uint8_t  jmp[3];
    char     oem[8];
    uint16_t bytes_per_sec;
    uint8_t  sec_per_cluster;
    uint16_t reserved_sec_count;
    uint8_t  num_fats;
    uint16_t root_ent_count;
    uint16_t total_sec_16;
    uint8_t  media;
    uint16_t fat_sz_16;
    uint16_t sec_per_track;
    uint16_t num_heads;
    uint32_t hidden_sec;
    uint32_t total_sec_32;
    uint32_t fat_sz_32;
    uint16_t ext_flags;
    uint16_t fs_ver;
    uint32_t root_cluster;
    uint16_t fs_info;
    uint16_t bk_boot_sec;
    uint8_t  reserved[12];
    uint8_t  drv_num;
    uint8_t  reserved1;
    uint8_t  boot_sig;
    uint32_t vol_id;
    char     vol_lab[11];
    char     fs_type[8];
} __attribute__((packed)) fat32_bpb_t;

#define PARTITION_0_START 2048

static uint32_t reserved_sectors = 32;
static uint32_t fat_count = 2;
static uint32_t sectors_per_fat = 0;
static uint32_t sectors_per_cluster = 1;
static uint32_t root_cluster_num = 2;

uint32_t cluster_to_lba(int partition, uint32_t cluster) {
    uint32_t partition_base = (partition == 0) ? PARTITION_0_START : 0;
    uint32_t data_region_lba = partition_base + reserved_sectors + (fat_count * sectors_per_fat);
    return data_region_lba + ((cluster - 2) * sectors_per_cluster);
}

void parse_fat_name(uint8_t* raw_name, char* dest) {
    int i, j = 0;
    for (i = 0; i < 8; i++) {
        if (raw_name[i] == ' ' || raw_name[i] == '\0') break;
        dest[j++] = raw_name[i];
    }
    if (raw_name[8] != ' ' && raw_name[8] != '\0') {
        dest[j++] = '.';
        for (i = 8; i < 11; i++) {
            if (raw_name[i] == ' ' || raw_name[i] == '\0') break;
            dest[j++] = raw_name[i];
        }
    }
    dest[j] = '\0';
}

int fat_name_match(uint8_t* fat_name, const char* name) {
    char parsed[13];
    parse_fat_name(fat_name, parsed);
    // Simple case-insensitive match could be better, but OpaqueSheep is simple.
    return streq(parsed, name);
}

uint32_t find_in_dir(uint32_t cluster, const char* path) {
    uint16_t sector_buffer[256];
    char segment[13];
    int seg_idx = 0;

    while (*path && *path != '/') {
        if (seg_idx < 12) segment[seg_idx++] = *path;
        path++;
    }
    segment[seg_idx] = '\0';

    uint32_t lba = cluster_to_lba(0, cluster);
    ata_read_sector(lba, sector_buffer);
    DirectoryEntry* entries = (DirectoryEntry*)sector_buffer;

    for (int i = 0; i < 16; i++) {
        if (entries[i].name[0] == 0x00) break;
        if (entries[i].name[0] == 0xE5) continue;

        if (fat_name_match(entries[i].name, segment)) {
            uint32_t found_cluster = (entries[i].cluster_high << 16) | entries[i].cluster_low;
            if (*path == '/') {
                return find_in_dir(found_cluster, path + 1);
            }
            return found_cluster;
        }
    }
    return 0;
}

void flist() {
    uint16_t sector_buffer[256];
    ata_read_sector(PARTITION_0_START, sector_buffer);
    fat32_bpb_t* bpb = (fat32_bpb_t*)sector_buffer;

    reserved_sectors = bpb->reserved_sec_count;
    fat_count = bpb->num_fats;
    sectors_per_fat = bpb->fat_sz_32;
    sectors_per_cluster = bpb->sec_per_cluster;
    root_cluster_num = bpb->root_cluster;

    uint32_t root_lba = cluster_to_lba(0, root_cluster_num);
    ata_read_sector(root_lba, sector_buffer);
    DirectoryEntry* entry = (DirectoryEntry*)sector_buffer;

    print("\n[DIRECTORY LISTING - Partition 0]\n");
    for (int i = 0; i < 16; i++) {
        if (entry[i].name[0] == 0x00) break;
        if (entry[i].name[0] == 0xE5) continue;
        if (entry[i].attributes & 0x0F) continue;

        char name[13];
        parse_fat_name(entry[i].name, name);
        print("- ");
        print(name);
        print("\n");
    }
}

// Efficiency Trinity: FSInfo Logic
void update_fsinfo(uint32_t next_free, uint32_t free_count) {
    uint16_t sector_buffer[256];
    ata_read_sector(PARTITION_0_START + 1, sector_buffer);
    fat32_fsinfo_t* fsinfo = (fat32_fsinfo_t*)sector_buffer;

    if (fsinfo->lead_sig == 0x41615252 && fsinfo->struct_sig == 0x61417272) {
        fsinfo->next_free = next_free;
        fsinfo->free_count = free_count;
        ata_write_sector(PARTITION_0_START + 1, sector_buffer);
    }
}

size_t real_fwrite(const char* path, const void* data, size_t size, size_t count) {
    // Allocation-Only FSInfo Update (Mocked for now)
    // update_fsinfo(new_next_free, new_free_count);
    (void)path; (void)data; (void)size;
    return count;
}

void fFormat(const char* label) {
    fat32_bpb_t bpb;
    for(int i=0; i<8; i++) bpb.oem[i] = ' ';
    bpb.bytes_per_sec = 512;
    bpb.sec_per_cluster = 1;
    bpb.reserved_sec_count = 32;
    bpb.num_fats = 2;
    bpb.fat_sz_32 = 512;
    bpb.root_cluster = 2;
    bpb.vol_id = 0x12345678;

    for(int i=0; i<11; i++) {
        if (label[i] == '\0') {
            for(int j=i; j<11; j++) bpb.vol_lab[j] = ' ';
            break;
        }
        bpb.vol_lab[i] = label[i];
    }

    ata_write_sector(PARTITION_0_START, (uint16_t*)&bpb);
    print("Partition 0 Formatted as OPAQUESHEEP FAT32\n");
}

static service_t fat_svc = {
    .type = SERVICE_FILESYS,
    .name = "FAT32_DISK",
    .write_file = real_fwrite
};

void fat32_init(void) {
    service_register(&fat_svc);
}
