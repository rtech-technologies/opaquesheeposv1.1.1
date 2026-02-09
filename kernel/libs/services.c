#include "sys.h"
#include "services.h"

const char *svc_get_os_name(void) {
    return "OpaqueSheep OS";
}

const char *svc_get_os_version(void) {
    return "1.1.1";
}

uint64_t svc_get_uptime(void) {
    // Stub: in a real OS, this would read a timer
    static uint64_t ticks = 0;
    return ticks++;
}

size_t svc_get_memory_usage(void) {
    // Stub: returns a mock value
    return 4096;
}

int streq(const char *s1, const char *s2) {
    if (!s1 || !s2) return 0;
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char *)s1 == *(unsigned char *)s2;
}

size_t strlen(const char *s) {
    if (!s) return 0;
    size_t len = 0;
    while (s[len]) len++;
    return len;
}

int str_startswith(const char *s, const char *prefix) {
    if (!s || !prefix) return 0;
    while (*prefix) {
        if (*prefix != *s) return 0;
        prefix++;
        s++;
    }
    return 1;
}

char *str_split_once(char *s, char delim) {
    if (!s) return NULL;
    while (*s) {
        if (*s == delim) {
            *s = '\0';
            return s + 1;
        }
        s++;
    }
    return NULL;
}
