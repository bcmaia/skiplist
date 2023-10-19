/**
 * @file types.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-10-09
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED

#include "tads/constants.h"

/**
 * @brief This primitive like type should be used in function return values to
 * convey the resulting status of the peformed operation. Use it to inform the
 * caller that the function was a success, an error ocurred, partial failure,
 * etc.
 *
 */
typedef enum {
    SUCCESS = 0,
    EOF_ERR = EOF,
    ERROR,
    NUL_ERR,
    ALLOC_ERR,
    UNRECOVERABLE,
    REPEATED_ENTRY_ERR,
    TOO_MUCH_ERR,
    ARR_IS_FULL_ERR,
    PARTIAL_FAILURE,
    NOT_FOUND_ERR,
    CRITICAL_ERR,
} status_t;

#endif