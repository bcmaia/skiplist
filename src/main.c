
#include <stdio.h>
#include <time.h>

#include "tads/item.h"
#include "tads/skiplist.h"

int main () {
    // Randomize
    srand(time(NULL));

    int n;
    printf("How many entries: ");
    if (EOF == scanf(" %d", &n) ) {
        printf("ERROR: Cannot read. dam.");
        return 1;
    } 

    SkipList* sk = skiplist_new();
    if (NULL == sk) {
        printf("ERROR: Could not create the skiplist");
        return 1;
    } 
    for (int i = 0; i < n; i++) {
        printf("Write an entry: ");
        Item* item = item_read();
        if (SUCCESS != skiplist_insert(sk, item)) { // err hanling
            item_del(&item); 
        }
    }

    printf("\nSkiplist is: \n");
    skiplist_debug_print(sk);

    skiplist_del(&sk);
    printf("\nAll Done! (%p) \n", (void*)sk);

    return 0;
}