/**
 * @file tad_constants.h
 * @brief This file contains constants that are used through out the tads.
 *
 * This header file contains project-specific constants that are used
 * throughout the codebase. It provides a central location for managing critical
 * constants.
 */

#ifndef CONSTANTS_H_INCLUDED
#define CONSTANTS_H_INCLUDED

#define MAX_WORD_SIZE (50)
#define MAX_DESCRIPTION_SIZE (140)

// This defines how an overflow in the read description process should be
// handled.
#define RECOVER_FROM_BIG_INPUT (1)

#endif // CONSTANTS_H_INCLUDED