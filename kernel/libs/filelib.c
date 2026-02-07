#include "filelib.h"

size_t fwrite(const void *data, size_t size, size_t count) {
    (void)data;
    (void)size;
    (void)count;
    return 0;
}

size_t fappend(const void *data, size_t size, size_t count) {
    (void)data;
    (void)size;
    (void)count;
    return 0;
}

size_t fread(void *data, size_t size, size_t count) {
    (void)data;
    (void)size;
    (void)count;
    return 0;
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
    (void)label;
    return -1;
}
