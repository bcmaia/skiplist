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
#include "utils/mathutils.h"
#include <stdio.h>
#include <stdlib.h>

//=============================================================================/
//=================|    Constants   |==========================================/
//=============================================================================/

// Defines the probability p that is used to model the distribution of the level
// of a new node being inserted on the skiplist.
#define SKIPLIST_PROB (0.5)

// If this is set to 1, then the skiplist will be limited to never increase its
// total height (the max level) by more than one at each insersion.
#define RAISE_ONLY_ONCE (0)

// You can choose to define SKIPLIST_MAX_HEIGHT and/or SKIPLIST_MAX_LENGTH. If
// defined, they will act as a hardlimit. If not defined, lenght will be limited
// by memory and height by one billion (since we cannot use limits.h). 
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
 * @brief Removes an item from the skiplist.
 *
 * @param skiplist A pointer to a skiplist.
 * @param item A pointer to the item that will be used as reference in the
 * search for the correct item to be removed.
 * @return Item* The removed item (with ownership) or NULL if it was not found
 * or if an error ocurred.
 *
 * @note it is up to you to delete the returned item.
 * @note the returned ptr and the item parameter ptr may be the same, but the
 * probabily are diferrent. If the item ptr is aready in the list (not the item
 * value, the ptr), then the pointers are going to be equal. Keep that in mind
 * to avoid double freeing the memory.
 */
Item *skiplist_remove(SkipList *skiplist, Item *item);

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
 * @brief Prints all the itens of the skiplist that starts with the character c.
 *
 * @param skiplist ptr to the skiplist.
 * @param c the character we are interested on.
 * @return status_t \c SUCCESS for a successiful print, \c ERROR if any errors
 * occurr.
 */
status_t skiplist_print(const SkipList *skiplist, const char c);

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

/**
 * @brief Updates the value of an item in a skiplist.
 *
 * Searches an skiplist for an item and updates it to a new value if it was
 * found.
 *
 * @param skiplist a ptr to the skiplist.
 * @param item the item we are searching and its new value.
 * @return status_t \c SUCCESS if the item was found and updated to a new value,
 * \c NUL_ERR if any critial pointer passed to the function is NULL, \c
 * ALLOC_ERR if there was an issue allocating memory during the execution of the
 * program, \c NOT_FOUND_ERR if the item is not found on the skiplist, \c ERROR
 * if an unidentified error ocurred during the trace and search phase.
 *
 * @note Keep in mind an item has a key, which is used to identify it, and a
 * value. The item you pass as a parameter should have a key to an item already
 * on the skiplist and a new value.
 */
status_t skiplist_update(SkipList *skiplist, Item *item);

/**
 * @brief Checks if the current configurantion of a skiplist is valid and has no
 * errors.
 *
 * Use this to debug the skiplist
 *
 * @param skiplist a ptr to the skiplist.
 * @return _Bool \c 1 if the skiplist if valid, \c 0 if the skiplist has one or
 * more errors (invalid).
 */
_Bool skiplist_debug_validate(const SkipList *skiplist);

#endif // SKIPLIST_H_DEFINED