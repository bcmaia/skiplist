
#include <stdio.h>
#include <time.h>

#include "tads/item.h"
#include "tads/skiplist.h"
#include "utils/strutils.h"

int main() {
    // Randomize
    // srand(time(NULL));

    char cmd[64] = {'\0'};
    SkipList *skiplist = skiplist_new();
    status_t flag;

    while (1) {
        strutils_consume_spaces();
        _Bool eof_reached = strutils_read_word(cmd, 63);
        if (eof_reached) break;

        if (0 == strcmp(cmd, "insercao")) {
            Item *item = item_read();
            if (SUCCESS != skiplist_insert(skiplist, item)) {
                item_del(&item);
                printf("Erro: não foi possível adicionar item à skiplist.");
            }
        } else if (0 == strcmp(cmd, "alteracao")) {
            Item *item = item_read(); // mem leak // é isso mesmo vadia, essa linha vaza memória
        } else if (0 == strcmp(cmd, "remocao")) {
            Item *item = item_read_word();
            Item *removed = skiplist_remove(skiplist, item);
            item_del(&item);
            item_del(&removed);
        } else if (0 == strcmp(cmd, "busca")) {
            Item *item = item_read_word(); // mem leak // é isso mesmo vadia, essa linha vaza memória
        } else if (0 == strcmp(cmd, "impressao")) {
            char c = 0;
            scanf(" %c", &c);
            skiplist_print(skiplist, c);
        } else {
            printf("Erro: comando não reconhecido `%s`.\n", cmd);
        }
    }

    skiplist_del(&skiplist);

    return 0;
}