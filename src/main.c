
#include <stdio.h>
#include <time.h>

#include "tads/item.h"
#include "tads/skiplist.h"
#include "utils/strutils.h"

#define INVALID_OP_MSG "OPERACAO INVALIDA\n"

int main() {
    // printf("\n");

    // Randomize
    // srand(time(NULL));
    // srand(1);

    // Initializations
    char cmd[64] = {'\0'};
    // long long i = 9998;
    SkipList *skiplist = skiplist_new();
    // clock_t start = clock();

    // Execution loop
    while (EOF != scanf(" %62s", cmd)) { // stop at eof
        // clock_t start_op = clock();

        // if (i < 0) printf("\nOperation [%s]\n", cmd);

        // Operation: Insertion
        if (0 == strcmp(cmd, "insercao")) {
            Item *item = item_read();
            // printf("INSERTING: [");
            // item_print(item);
            // printf("];\n");

            if (SUCCESS != skiplist_insert(skiplist, item)) {
                item_del(&item);
                printf(INVALID_OP_MSG);
            }
            // if (i < 0) {
            // printf("INSERTED: [");
            //  item_print(item);
            // printf("]; \n");
            // }
        }

        // Operation: Update
        else if (0 == strcmp(cmd, "alteracao")) {
            Item *item = item_read();

            // printf("UPDATING: [");
            // item_print(item);
            // printf("]; \n");

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

            // printf("SEARCHING: [");
            //  item_print(item);
            // printf("]; \n");

            // printf("FOUND: [");
            //  item_print(result);
            // printf("]; \n");

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
            // printf("IVALID OP [%s]\n", cmd);
        }

        // DEBUG INFO
        // if (!skiplist_debug_validate(skiplist)) {
        //     printf("INVALID SKIPLIST!!!\n");
        //     break;
        // }
        // i--;
        // clock_t end_op = clock();
        // double t = ((double)(end_op - start_op)) / CLOCKS_PER_SEC;
        // printf(
        //     "Operation [%s]: time(%lf) size(%lu) height(%lu)\n", cmd, t,
        //     skiplist_length(skiplist), skiplist_height(skiplist)
        // );
    }
    // printf("\n");

    // DEBUG print time
    // clock_t end = clock();
    // double cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    // printf("CPU time used: %f seconds\n", cpu_time_used);
    // printf("\n");

    // Clean up
    skiplist_del(&skiplist);

    return 0;
}