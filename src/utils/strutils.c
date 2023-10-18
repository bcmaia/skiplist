#include "utils/strutils.h"

/**
 * @brief This function will read from the stdin until a new line ('\n') is
 * read. All the characters it reads will be discarted.
 *
 */
void strutils_consume_line(void) {
    char c;
    while (EOF != (c = getchar()) && '\n' != c)
        ;
}

/**
 * @brief This function will read characters from stdin until the first white
 * space is encountered. All the characters read will be discarded.
 */
void strutils_consume_word(void) {
    char c;
    while (EOF != (c = getchar()) && !strutils_isspace(c))
        ;
    if (EOF != c && '\n' != c) ungetc(c, stdin);
}

/**
 * @brief This function will read characters from stdin until the first white
 * space is encountered. All the characters read will be discarded.
 */
void strutils_consume_spaces(void) {
    char c;
    while (EOF != (c = getchar()) && strutils_isspace(c))
        ;
    if (EOF != c && !strutils_isspace(c)) ungetc(c, stdin);
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

// _Bool strutils_read_word(char s[], const size_t max_size) {
//     char c;
//     int i = 0;
//     for (;i < max_size && EOF != (c = getchar()) && !strutils_isspace(c); i++) 
//         s[i] = c;

//     s[i] = '\0';
//     return EOF == c;
// }