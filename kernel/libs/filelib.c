#include "filelib.h"
#include "sys.h"

// Very simple in-memory "virtual disk" to simulate FAT working
static char vfs_buffer[4096];
static size_t vfs_size = 0;
static char vfs_label[32] = "NONE";

size_t fwrite(const void *data, size_t size, size_t count) {
    size_t bytes = size * count;
    if (bytes > 4096) bytes = 4096;
    memcpy_simple(vfs_buffer, data, bytes);
    vfs_size = bytes;
    return count;
}

size_t fappend(const void *data, size_t size, size_t count) {
    size_t bytes = size * count;
    if (vfs_size + bytes > 4096) bytes = 4096 - vfs_size;
    memcpy_simple(vfs_buffer + vfs_size, data, bytes);
    vfs_size += bytes;
    return count;
}

size_t fread(void *data, size_t size, size_t count) {
    size_t bytes = size * count;
    if (bytes > vfs_size) bytes = vfs_size;
    memcpy_simple(data, vfs_buffer, bytes);
    return bytes / size;
}

int fdelete(const char *path) {
    (void)path;
    return -1;
}

int dmake(const char *path) {
    (void)path;
    return -1;
}

int dremove(const char *path) {
    (void)path;
    return -1;
}

int dmove(const char *from, const char *to) {
    (void)from;
    (void)to;
    return -1;
}

int fFormat(const char *label) {
    memset_simple(vfs_buffer, 0, 4096);
    vfs_size = 0;
    if (label) {
        // Copy label (simple)
        for(int i=0; i<31 && label[i]; i++) vfs_label[i] = label[i];
    }
    print("Filesystem formatted: ");
    print(vfs_label);
    print("\n");
    return 0;
}
