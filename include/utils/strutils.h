#ifndef STRUTILS_H_INCLUDED
#define STRUTILS_H_INCLUDED

#include <stdio.h>
// #include <string.h>

void strutils_consume_line(void); 
void strutils_consume_word(void);
void strutils_consume_spaces(void);

char strutils_isspace(const char c);
char strutils_isspace_not_newline(const char c);


#endif // STRUTILS_H_INCLUDED