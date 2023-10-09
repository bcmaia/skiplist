#ifndef SKIPLIST_H_DEFINED
#define SKIPLIST_H_DEFINED

#include <stdio.h>
#include <stdlib.h>
#include "tads/types.h"
#include "tads/item.h"

// TODO
// 1. Define skiplist_len() and use it on insert validation

// NOTE (b): If no SKIPLIST_MAX_HEIGHT is defined, tha we assume the skiplist
// can grow forever. This is not actualy that bad. Because of how probability
// works, it will likely not reach the skies.
// #define SKIPLIST_MAX_HEIGHT (128)

typedef struct _skiplist_s SkipList;

SkipList *skiplist_new(void);
void skiplist_del(SkipList **skiplist);
status_t skiplist_insert(SkipList *skiplist, Item *item);
void skiplist_debug_print(const SkipList *skiplist);

#endif // SKIPLIST_H_DEFINED