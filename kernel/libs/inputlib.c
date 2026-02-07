#include "inputlib.h"

const char *input(const char *prompt) {
    static char buffer[1] = { '\0' };
    if (prompt) {
        print(prompt);
    }
    return buffer;
}
