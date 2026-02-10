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

/**
 * exec_line - Command Dispatcher for RSL (Rtech Script Language)
 * Processes a single line of commands.
 */
void exec_line(char *line) {
    if (!line || line[0] == '\0') return;

    char *args = str_split_once(line, ' ');
    const char *cmd = line;

    if (streq(cmd, "help")) {
        print("help   - list commands\n");
        print("info   - system status\n");
        print("ls     - list files\n");
        print("cat    - read file content\n");
        print("touch  - create empty file\n");
        print("rm     - delete file\n");
        print("echo   - write text to file\n");
        print("uptime - show system uptime\n");
        print("run    - execute RSL script\n");
        print("format - format virtual disk\n");
        print("panic  - test fatal error\n");
        print("clear  - clear screen\n");
    }

    else if (streq(cmd, "info")) {
        print("OS: "); print(svc_get_os_name()); print("\n");
        print("Version: "); print(svc_get_os_version()); print("\n");
        print("Memory Used: ");
        if (svc_get_memory_usage() > 0) print("4096 KB\n");
    }

    else if (streq(cmd, "ls")) {
        flist();
    }

    else if (streq(cmd, "cat")) {
        if (!args) {
            print("Usage: cat <path>\n");
        } else {
            char buf[512];
            memset_simple(buf, 0, 512);
            size_t r = fread(args, buf, 1, 511);
            if (r > 0) {
                print(buf);
                print("\n");
            } else {
                print("File not found or empty.\n");
            }
        }
    }

    else if (streq(cmd, "touch")) {
        if (!args) {
            print("Usage: touch <path>\n");
        } else {
            fwrite(args, "", 1, 0);
            print("File touched.\n");
        }
    }

    else if (streq(cmd, "rm")) {
        if (!args) {
            print("Usage: rm <path>\n");
        } else {
            if (fdelete(args) == 0) print("File deleted.\n");
            else print("Error deleting file.\n");
        }
    }

    else if (streq(cmd, "echo")) {
        if (!args) {
            print("Usage: echo <path> <text>\n");
        } else {
            char *text = str_split_once(args, ' ');
            if (!text) {
                print("Usage: echo <path> <text>\n");
            } else {
                fwrite(args, text, 1, strlen(text));
                print("Written to "); print(args); print("\n");
            }
        }
    }

    else if (streq(cmd, "uptime")) {
        print("Uptime: ");
        if (svc_get_uptime() > 0) print("Active\n");
        else print("0 ticks\n");
    }

    else if (streq(cmd, "run")) {
        if (!args) {
            print("Usage: run <path>\n");
        } else {
            run(args);
        }
    }

    else if (streq(cmd, "format")) {
        fFormat("OPAQUESHEEP");
    }

    else if (streq(cmd, "panic")) {
        panic("User requested fatal error.");
    }

    else if (streq(cmd, "clear")) {
        iolib_clear(0x000000);
        iolib_reset_cursor();
    }

    else if (cmd[0] != '\0') {
        print("Unknown command: ");
        print(cmd);
        print("\n");
    }
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

/**
 * run - Executes RSL script file line-by-line
 */
void run(const char *path) {
    if (!path) return;
    char buf[1024];
    memset_simple(buf, 0, 1024);
    size_t r = fread(path, buf, 1, 1023);
    if (r == 0) return;

    char *line = buf;
    for (size_t i = 0; i < r; i++) {
        if (buf[i] == '\n' || buf[i] == '\r') {
            buf[i] = '\0';
            exec_line(line);
            line = &buf[i+1];
        }
    }
    if (line < buf + r) {
        exec_line(line);
    }
}
