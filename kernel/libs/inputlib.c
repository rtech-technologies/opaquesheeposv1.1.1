#include "inputlib.h"
#include "sys.h"

const char *input(const char *prompt) {
    static char buffer[256];
    size_t index = 0;

    if (prompt) {
        print(prompt);
    }

    for (;;) {
        char c = iolib_getc();

        if (c == '\n' || c == '\r') {
            buffer[index] = '\0';
            print("\n");
            return buffer;
        } else if (c == '\b' || c == 127) {
            if (index > 0) {
                index--;
                print("\b \b");
            }
        } else if (index < 255) {
            buffer[index++] = c;
            char str[2] = {c, '\0'};
            print(str);
        }
    }
}
