#ifndef STRUTILS_H_INCLUDED
#define STRUTILS_H_INCLUDED

#include <stdio.h>
// #include <string.h>

void strutils_consume_line(void); 
void strutils_consume_word(void);
int strutils_consume_spaces(void);

_Bool strutils_read_word(char s[], const size_t max_size);

_Bool strutils_isspace(const char c);
_Bool strutils_isspace_not_newline(const char c);


#endif // STRUTILS_H_INCLUDED