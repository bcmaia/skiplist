/**
 * @file skiplist.h
 * @author Bernardo M. C. (b), (bernardomc@usp.br, bcmaiax@gmail.com)
 * @brief this header declares a abstract data structure `skiplist` and the
 * functions that can be used with it.
 * @version 0.1
 * @date 2023-10-09
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef SKIPLIST_H_DEFINED
#define SKIPLIST_H_DEFINED

//=============================================================================/
//=================|    Dependencies    |======================================/
//=============================================================================/

#include "tads/item.h"
#include "tads/types.h"
#include <stdio.h>
#include <stdlib.h>

//=============================================================================/
//=================|    Constants   |==========================================/
//=============================================================================/

// NOTE (b): you can choose to define SKIPLIST_MAX_HEIGHT and/or
// SKIPLIST_MAX_LENGTH or not.
// #define SKIPLIST_MAX_HEIGHT (128)
// #define SKIPLIST_MAX_LENGTH (1024)

//=============================================================================/
//=================|    Types     |============================================/
//=============================================================================/
/**
 * @brief An incomplete wrapper for the Skiplist structure.
 *
 * An incomplete wrapper for the Skiplist structure. You can only use it
 * indirectly by having it as a ptr.
 *
 */
typedef struct _skiplist_s SkipList;

//=============================================================================/
//=================|    Functions     |========================================/
//=============================================================================/

/**
 * @brief Creates a new heap-allocated empty skiplist.
 *
 * @return SkipList* ptr to the skip list, WITH OWNERSHIP, or NULL in case of
 * error.
 */
SkipList *skiplist_new(void);

/**
 * @brief An opinionated function to delete a skiplist (it cleans the memory
 * used by it).
 *
 * @param skiplist a ptr to the skiplist.
 *
 * @note this will set your ptr to NULL to avoid dangling ptrs.
 */
void skiplist_del(SkipList **skiplist);

/**
 * @brief This function inserts an item into the skiplist.
 *
 * @param skiplist a ptr to the skip list.
 * @param item a ptr to the item.
 * @return status_t
 *
 * @warning make sure the ptr are valid. NULL ptrs will be caught as errors,
 *  but other tipe of invalid pointers will not.
 *
 * @note it uses rand as source of randomness.
 */
status_t skiplist_insert(SkipList *skiplist, Item *item);

/**
 * @brief Searched for an item in the skiplist.
 *
 * @param skiplist ptr to the skiplist.
 * @param item ptr to the item.
 * @return Item* NULL if nothing is found or, if founded, a ptr to the item
 * WITHOUT OWNERSHIP.
 */
Item *skiplist_search(const SkipList *skiplist, const Item *item);

/**
 * @brief prints all the levels of the skiplist and some debug info. Use it to
 * aid debugging.
 *
 * @param skiplist ptr to the skiplist.
 */
void skiplist_debug_print(const SkipList *skiplist);

/**
 * @brief Checks if the skip list has reach maximun lengyh.
 *
 * @param skiplist ptr to the list.
 * @return _Bool \c 1 if max length was reached, \c 0 otherwise.
 *
 * @note if SKIPLIST_MAX_LENGTH is not defined, this will always return flase.
 */
_Bool skiplist_is_full(const SkipList *skiplist);

/**
 * @brief Checks if the list is empty
 *
 * @param skiplist ptr to the list.
 * @return _Bool \c 1 if the list is empty, \c 0 otherwise.
 */
_Bool skiplist_is_empty(const SkipList *skiplist);

/**
 * @brief A getter to the skiplist length.
 *
 * @param skiplist a ptr to the skiplist.
 * @return size_t the length of the skiplist.
 */
size_t skiplist_length(const SkipList *skiplist);

/**
 * @brief A getter to the skiplist height.
 *
 * @param skiplist a ptr to the skiplist.
 * @return size_t the height of the skiplist.
 */
size_t skiplist_height(const SkipList *skiplist);

#endif // SKIPLIST_H_DEFINED