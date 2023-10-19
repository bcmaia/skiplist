#include "utils/strutils.h"

void strutils_consume_line(void) {
    char c;
    while (EOF != (c = getchar()) && '\n' != c)
        ;
}

void strutils_consume_word(void) {
    char c;
    while (EOF != (c = getchar()) && !strutils_isspace(c))
        ;
    if (EOF != c) ungetc(c, stdin);
}

int strutils_consume_spaces(void) {
    char c;
    while (EOF != (c = getchar()) && strutils_isspace(c))
        ;
    if (EOF == c) return -1;
    if (!strutils_isspace(c)) ungetc(c, stdin);
    return 0;
}

_Bool strutils_isspace(const char c) {
    return (
        c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v' ||
        c == '\f'
    );
}

_Bool strutils_isspace_not_newline(const char c) {
    return (c == ' ' || c == '\t' || c == '\r' || c == '\v' || c == '\f');
}