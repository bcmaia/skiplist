
#include <stdio.h>
#include <time.h>

#include "tads/item.h"
#include "tads/skiplist.h"
#include "utils/strutils.h"

#define INVALID_OP_MSG "OPERACAO INVALIDA\n"

int main() {
    printf("\n");

    // Randomize
    // srand(time(NULL));
    // srand(1);

    // Initializations
    char cmd[64] = {'\0'};
    SkipList *skiplist = skiplist_new();
    // clock_t start = clock();

    // Execution loop
    while (EOF != scanf(" %62s", cmd)) { // stop at eof
        // clock_t start_op = clock();

        // printf("Operation [%s]\n", cmd);

        // Operation: Insertion
        if (0 == strcmp(cmd, "insercao") || 0 == strcmp(cmd, "i")) {
            Item *item = item_read();
            // printf("INSERTING: [");
            // item_print(item);
            // printf("];\n");

            if (SUCCESS != skiplist_insert(skiplist, item)) {
                item_del(&item);
                printf("1" INVALID_OP_MSG);
            }
        }

        // Operation: Update
        else if (0 == strcmp(cmd, "alteracao") || 0 == strcmp(cmd, "a")) {
            Item *item = item_read();

            if (SUCCESS != skiplist_update(skiplist, item))
                printf("2" INVALID_OP_MSG);

            item_del(&item);
        }

        // Operation: Remove
        else if (0 == strcmp(cmd, "remocao") || 0 == strcmp(cmd, "r")) {
            Item *item = item_read_word();
            Item *removed = skiplist_remove(skiplist, item);

            if (NULL == removed) printf("3" INVALID_OP_MSG);

            item_del(&item);
            item_del(&removed);
        }

        // Operation: Search
        else if (0 == strcmp(cmd, "busca") || 0 == strcmp(cmd, "b")) {
            Item *item = item_read_word();                  // owned
            Item *result = skiplist_search(skiplist, item); // borrowed

            // Print and error handling
            if (NULL == item || NULL == result) {
                printf("4" INVALID_OP_MSG);
            } else {
                item_print(result);
                printf("\n");
            }

            item_del(&item);
        }

        // Operation: Print
        else if (0 == strcmp(cmd, "impressao") || 0 == strcmp(cmd, "imp")) {
            // Input
            char c = 0;
            int _flag = scanf(" %c", &c);

            // Error handling
            if (EOF == _flag) {
                printf("5" INVALID_OP_MSG);
                break;
            }

            // output
            status_t _flag_2 = skiplist_print(skiplist, c);
            if (SUCCESS != _flag_2) printf("6" INVALID_OP_MSG);
        }

        // Operation: Operation not identified
        else {
            printf("7" INVALID_OP_MSG);
        }

        // DEBUG INFO
        // clock_t end_op = clock();
        // double t = ((double)(end_op - start_op)) / CLOCKS_PER_SEC;
        // printf(
        //     "Operation [%s]: time(%lf) size(%lu) height(%lu)\n", cmd, t,
        //     skiplist_length(skiplist), skiplist_height(skiplist)
        // );

        // printf("SKIPLIST:\n");
        // skiplist_debug_print(skiplist);
        // printf("\n\n");
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