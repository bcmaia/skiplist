#ifndef SKIPLIST_H_DEFINED
#define SKIPLIST_H_DEFINED

#include <stdlib.h>
#include <stdio.h>
#include "tads/item.h"

typedef struct _skiplist_s SkipList;

SkipList* skiplist_new (void);
void skiplist_del (SkipList** skiplist);
status_t skiplist_insert(SkipList *skiplist, Item *item);
void skiplist_debug_print(SkipList *skiplist);

#endif // SKIPLIST_H_DEFINED