#include "sys.h"
#include "filelib.h"

#define MAX_FILES 8
#define FILE_BUF_SIZE 4096

typedef struct {
    char name[32];
    char data[FILE_BUF_SIZE];
    size_t size;
    int used;
} vfs_entry_t;

static vfs_entry_t vfs_table[MAX_FILES];
static char vfs_label[32] = "OPAQUESHEEP";

size_t fwrite(const char *path, const void *data, size_t size, size_t count) {
    if (!path) return 0;
    size_t bytes = size * count;
    if (bytes > FILE_BUF_SIZE) bytes = FILE_BUF_SIZE;

    // Find if file exists
    int slot = -1;
    for (int i = 0; i < MAX_FILES; i++) {
        if (vfs_table[i].used && streq(vfs_table[i].name, path)) {
            slot = i;
            break;
        }
    }

    // If not, find empty slot
    if (slot == -1) {
        for (int i = 0; i < MAX_FILES; i++) {
            if (!vfs_table[i].used) {
                slot = i;
                vfs_table[i].used = 1;
                for (int j = 0; j < 31 && path[j]; j++) vfs_table[i].name[j] = path[j];
                break;
            }
        }
    }

    if (slot != -1) {
        memcpy_simple(vfs_table[slot].data, data, bytes);
        vfs_table[slot].size = bytes;
        return count;
    }

    return 0;
}

size_t fappend(const char *path, const void *data, size_t size, size_t count) {
    if (!path) return 0;
    size_t bytes = size * count;

    for (int i = 0; i < MAX_FILES; i++) {
        if (vfs_table[i].used && streq(vfs_table[i].name, path)) {
            if (vfs_table[i].size + bytes > FILE_BUF_SIZE) {
                bytes = FILE_BUF_SIZE - vfs_table[i].size;
            }
            memcpy_simple(vfs_table[i].data + vfs_table[i].size, data, bytes);
            vfs_table[i].size += bytes;
            return bytes / size;
        }
    }
    return 0;
}

size_t fread(const char *path, void *data, size_t size, size_t count) {
    if (!path) return 0;
    size_t bytes = size * count;

    for (int i = 0; i < MAX_FILES; i++) {
        if (vfs_table[i].used && streq(vfs_table[i].name, path)) {
            if (bytes > vfs_table[i].size) bytes = vfs_table[i].size;
            memcpy_simple(data, vfs_table[i].data, bytes);
            return bytes / size;
        }
    }
    return 0;
}

int fdelete(const char *path) {
    if (!path) return -1;
    for (int i = 0; i < MAX_FILES; i++) {
        if (vfs_table[i].used && streq(vfs_table[i].name, path)) {
            vfs_table[i].used = 0;
            memset_simple(vfs_table[i].name, 0, 32);
            return 0;
        }
    }
    return -1;
}

void flist(void) {
    print("Listing files in /:\n");
    int count = 0;
    for (int i = 0; i < MAX_FILES; i++) {
        if (vfs_table[i].used) {
            print("- ");
            print(vfs_table[i].name);
            print(" (");
            // print size as mock string since we don't have itoa yet
            if (vfs_table[i].size == 0) print("0");
            else print("...");
            print(" bytes)\n");
            count++;
        }
    }
    if (count == 0) print("(empty)\n");
}

int fFormat(const char *label) {
    for (int i = 0; i < MAX_FILES; i++) {
        vfs_table[i].used = 0;
    }
    if (label) {
        for(int i=0; i<31 && label[i]; i++) vfs_label[i] = label[i];
    }
    print("VFS Formatted: ");
    print(vfs_label);
    print("\n");
    return 0;
}

int dmake(const char *path) { (void)path; return -1; }
int dremove(const char *path) { (void)path; return -1; }
int dmove(const char *from, const char *to) { (void)from; (void)to; return -1; }
