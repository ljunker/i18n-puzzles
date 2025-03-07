#include <string.h>

size_t calc_char_length(const char *str) {
    size_t char_length = 0;
    while (*str) {
        if ((*str & 0xC0) != 0x80) {
            char_length++;
        }
        str++;
    }
    return char_length;
}

void trim_newline(char *str) {
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
}
