#include "services.h"

typedef struct {
    int drive;      // e.g., 0
    int partition;  // e.g., 0
    const char* filename;
} ParsedPath;

ParsedPath parse_os_path(const char* path) {
    ParsedPath p = {0, 0, path};

    // Check for the "Drive/:Partition/" prefix (e.g. 0/:0/)
    if (path[0] >= '0' && path[0] <= '9' &&
        path[1] == '/' && path[2] == ':' &&
        path[3] >= '0' && path[3] <= '9' &&
        path[4] == '/') {
        p.drive = path[0] - '0';
        p.partition = path[3] - '0';
        p.filename = &path[5]; // The actual file path starts here
    }
    return p;
}
