/**
 * @file strutils.h
 * @brief Header file for string utility functions.
 *
 * This header defines utility functions for working with strings, including
 * operations like string manipulation and parsing.
 */


#ifndef STRUTILS_H_INCLUDED
#define STRUTILS_H_INCLUDED

#include <stdio.h>

/**
 * @brief This function will read from the stdin until a new line ('\n') is
 * read. All the characters it reads will be discarted.
 *
 */
void strutils_consume_line(void); 

/**
 * @brief This function will read characters from stdin until the first white
 * space is encountered. All the characters read will be discarded.
 */
void strutils_consume_word(void);

/**
 * @brief This function will read characters from stdin until the first white
 * space is encountered. All the characters read will be discarded.
 */
int strutils_consume_spaces(void);

/**
 * @brief This function will return true if the given character is a white space
 * character. It will return false otherwise.
 *
 * @param c the character to be tested.
 * @return _Bool true if the given character is a white space character. It will
 * return false otherwise.
 */

_Bool strutils_isspace(const char c);

/**
 * @brief This function will return true if the given character is a white space
 * character that is not a new line ('\n'). It will return false otherwise.
 *
 * @param c the character to be tested.
 * @return _Bool true if the given character is a white space character that is
 * not a new line ('\n'). It will return false otherwise.
 */
_Bool strutils_isspace_not_newline(const char c);


#endif // STRUTILS_H_INCLUDED