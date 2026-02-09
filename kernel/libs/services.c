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
