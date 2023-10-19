/**
 * @file tad_types.h
 * @brief Header file for custom data types and enumerations.
 *
 * This header file defines custom data types and enumerations used
 * throughout the project. These types are designed to enhance code
 * clarity and maintainability by giving meaningful names to data.
 *
 */

#ifndef TAD_TYPES_H_INCLUDED
#define TAD_TYPES_H_INCLUDED

#include "tads/tad_constants.h"

/**
 * @brief This primitive like type should be used in function return values to
 * convey the resulting status of the peformed operation. Use it to inform the
 * caller that the function was a success, an error ocurred, partial failure,
 * etc.
 *
 */
typedef enum {
    SUCCESS = 0,
    EOF_ERR = -1,
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

#endif // TAD_TYPES_H_INCLUDED