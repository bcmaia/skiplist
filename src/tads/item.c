#include "tads/item.h"

struct _item_s {
    entry_t val;
};

Item *item_new(const entry_t entry) {
    Item *item = (Item *)malloc(sizeof(Item));
    if (item) item->val = entry;
    return item;
}

void item_del(Item **item) {
#if DEBUG
    printf("DELETING [%s] \n", (*item)->val.word);
#endif
    free(*item);
    *item = NULL;
}

int item_cmp(const Item *item_1, const Item *item_2) {
    if (NULL == item_1 || NULL == item_2) return 0;
    return strcmp(item_1->val.word, item_2->val.word);
}

// TODO: finish this func
void item_print(Item *item) { printf("%s", item ? item->val.word : "<NULL>"); }

void item_print_word(Item *item) {
#ifdef RELEASE
    printf("%s", item->val.word); // if release, trust and optimize
#else
    printf("%s", item ? item->val.word : "<NULL>");
#endif
}

void item_print_description(Item *item) {
#ifdef RELEASE
    printf("%s", item->val.description); // if release, trust and optimize
#else
    printf("%s", item ? item->val.description : "<NULL>");
#endif
}

Item *item_read(void) {
#define CANARY (-1)

    entry_t e;
    e.description[MAX_DESCRIPTION_SIZE] = CANARY;

    scanf(WORD_SCN, e.word);
    scanf(DESCRIPTION_SCN, e.description);

    // If we read 140 characters, than lets consume the rest of the line
    if (CANARY != e.description[MAX_DESCRIPTION_SIZE]) strutils_consume_line();

    return item_new(e);
}