
#include <stdio.h>
#include "tads/item.h"
#include "tads/skiplist.h"

int main () {
    int n;
    printf("How many entries: ");
    scanf(" %d", &n);

    SkipList* sk = skiplist_new();
    for (int i = 0; i < n; i++) {
        printf("\nWrite an entry: ");
        Item* item = item_read();
        skiplist_insert(sk, item);
    }

    printf("\nSkiplist is: ");
    skiplist_debug_print(sk);

    skiplist_del(&sk);
    printf("\nAll Done!\n");

    return 0;
}