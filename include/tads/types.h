#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED

#include "tads/constants.h"

typedef enum {
    SUCCESS = 0,
    ERROR = 1,
    NUL_ERR = 2,
    ALLOC_ERR = 3,
    REPEATED_ENTRY_ERR = 4,
} status_t;

typedef struct {
    // NOTE (b): Remember to acount for the extra '\0' character
    char word[1 + MAX_WORD_SIZE]; 
    char description[1 + MAX_DESCRIPTION_SIZE];
} entry_t;

#endif