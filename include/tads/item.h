/**
 * @file item.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-10-09
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef ITEM_H_DEFINED
#define ITEM_H_DEFINED

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tads/types.h"
#include "tads/constants.h"
#include "utils/strutils.h"

/**
 * @brief the implementation of the item data structure. This is private to this
 * cfile.
 *
 * @note the atributes of this structure are stack stored.
 */
typedef struct _item_s Item;

/**
 * @brief Creates a new item on the heap and returns a ptr to it.
 *
 * @return a pointer to the item.
 */
Item *item_new(void);

/**
 * @brief deletes
 *
 * @param item a pointer to the item.
 */
void item_del(Item **item);

/**
 * @brief Compares two itens.
 *
 * It is almost just a wrapper around strcmp. Keep in mind it does not check if
 * the pointers are null.
 *
 * @param item_1 ptr to an item.
 * @param item_2 ptr to an item.
 * @return int y. y = 0 for equal items. y < 0 when item_2 is ordered
 * alfabatically after item_1 and y > 0 otherwise.
 *
 */
int item_raw_cmp(const Item *item_1, const Item *item_2);

/**
 * @brief Compares two itens.
 *
 * It is almost just a wrapper around strcmp; but it checks if the pointers are
 * null.
 *
 * @param item_1 ptr to an item.
 * @param item_2 ptr to an item.
 * @return int y. y = 0 for equal items. y < 0 when item_2 is ordered
 * alfabatically after item_1 and y > 0 otherwise.
 *
 */
int item_cmp(const Item *item_1, const Item *item_2);

/**
 * @brief Prints an item.
 *
 * Receives a pointer to an item and prints the word and the description,
 * separated by a space.
 *
 * @param item ptr to an item.
 *
 */
void item_print(const Item *item);

/**
 * @brief prints the word atribute found in the item entry.
 *
 * @param item ptr to the item.
 */
void item_print_word(const Item *item);

/**
 * @brief prints the description atribute found in the item entry.
 *
 * @param item ptr to the item.
 */
void item_print_description(const Item *item);

/**
 * @brief reads and constructs a item on the heap based on a user input from
 * stdin. It will read an input in the format `{W} {D}`, where W is a string
 * with no white space and D is a string that can include white spaces. A new
 * line "\n" will determine the end of the reading process.
 *
 * @return Item*
 *
 */
Item *item_read(void);

/**
 * @brief Reads a word from stdin and returns a pointer to a new item with the
 * word as its word atribute.
 * 
 * @return Item* a pointer to a new item with the word as its word atribute.
*/
Item *item_read_word(void);

/**
 * @brief Compares the first character of the word with the given char.
 * 
 * @param item a pointer to the item, 
 * @param c the char to be compared.
 * @return int if(>0) => item->val.word[0] > c, 
 * if(0) => item->val.word[0] == c,
 * if(<0) => item->val.word[0] < c
*/
int item_raw_char_cmp (const Item* item, const char c);

/**
 * @brief Updates the destiny item with the reference item.
 * 
 * @param destiny ptr to the item that will be updated.
 * @param reference ptr to the item that will be used as reference.
*/
void item_raw_update (Item* destiny, const Item* reference);

#endif // ITEM_H_DEFINED