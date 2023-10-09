#ifndef ITEM_H_DEFINED
#define ITEM_H_DEFINED

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tads/types.h"
#include "tads/constants.h"
#include "utils/strutils.h"

typedef struct _item_s Item;

Item *item_new(void);
void item_del(Item **item);
int item_cmp(const Item *item_1, const Item *item_2);
void item_print(const Item *item);
void item_print_word(const Item *item);
void item_print_description(const Item *item);
Item *item_read(void);

#endif // ITEM_H_DEFINED