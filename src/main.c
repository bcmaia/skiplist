#include <stdio.h>
#include <time.h>

#include "tads/item.h"
#include "tads/skiplist.h"
#include "utils/strutils.h"

#define INVALID_OP_MSG "OPERACAO INVALIDA\n"

int main() {

    // Randomize
    srand(time(NULL));

    // Initializations
    char cmd[64] = {'\0'};

    SkipList *skiplist = skiplist_new();

    // Execution loop
    while (EOF != scanf(" %62s", cmd)) { // stop at eof

        // Operation: Insertion
        if (0 == strcmp(cmd, "insercao")) {
            Item *item = item_read();

            if (SUCCESS != skiplist_insert(skiplist, item)) {
                item_del(&item);
                printf(INVALID_OP_MSG);
            }
        }

        // Operation: Update
        else if (0 == strcmp(cmd, "alteracao")) {
            Item *item = item_read();

            if (SUCCESS != skiplist_update(skiplist, item))
                printf(INVALID_OP_MSG);

            item_del(&item);
        }

        // Operation: Remove
        else if (0 == strcmp(cmd, "remocao")) {
            Item *item = item_read_word();

            Item *removed = skiplist_remove(skiplist, item);

            if (NULL == removed) printf(INVALID_OP_MSG);

            item_del(&item);
            item_del(&removed);
        }

        // Operation: Search
        else if (0 == strcmp(cmd, "busca")) {
            Item *item = item_read_word();                  // owned
            Item *result = skiplist_search(skiplist, item); // borrowed

            // Print and error handling
            if (NULL == item || NULL == result) {
                printf(INVALID_OP_MSG);
            } else {
                item_print(result);
                printf("\n");
            }

            item_del(&item);
        }

        // Operation: Print
        else if (0 == strcmp(cmd, "impressao")) {
            // Input
            char c = 0;
            int _flag = scanf(" %c", &c);

            // Error handling
            if (EOF == _flag) {
                printf(INVALID_OP_MSG);
                break;
            }

            // output
            status_t _flag_2 = skiplist_print(skiplist, c);
            if (SUCCESS != _flag_2) printf(INVALID_OP_MSG);
        }

        else if (0 == strcmp(cmd, "debug")) {
            printf("SKIPLIST:\n");
            skiplist_debug_print(skiplist);
            printf("\n\n");
        }

        // Operation: Operation not identified
        else {
            printf(INVALID_OP_MSG);
        }
    }

    // Clean up
    skiplist_del(&skiplist);

    return 0;
}