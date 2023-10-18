#include "tads/item.h"

//============================================================================//
//=================|    Data Structures with Implementations    |=============//
//============================================================================//

/**
 * @brief And dictionary entry. This is a stack type to store the word string
 * and the description string.
 *
 */
typedef struct {
    // NOTE (b): Remember to acount for the extra '\0' character
    char word[1 + MAX_WORD_SIZE];
    char description[1 + MAX_DESCRIPTION_SIZE];
} entry_t;

/**
 * @brief the implementation of the item data structure. This is private to this
 * cfile.
 *
 * @note the atributes of this structure are stack stored.
 */
struct _item_s {
    entry_t val;
};

//============================================================================//
//=================|    Private Function Declarations    |====================//
//============================================================================//

Item *item_from_entry(const entry_t entry);
Item *item_from_strings(const char w[], const char d[]);
status_t read_word(char s[]);
status_t read_description(char s[]);

//============================================================================//
//=================|    Public Function Implementations    |==================//
//============================================================================//

Item *item_new(void) {
    Item *item = (Item *)malloc(sizeof(Item)); // allocation
    if (NULL == item) return NULL;             // err handling

    // Initialization
    item->val.word[0] = '\0';
    item->val.description[0] = '\0';

    return item;
}

/**
 * @brief deletes
 *
 * @param item a pointer to the item.
 */
void item_del(Item **item) {
    if (NULL == item) return; // err handling
    // NOTE: free(NULL) is fine, so no need to check *item.

    // Debug printing if needed.
    // #if DEBUG
    //     printf("DELETING [%s] \n", (*item)->val.word);
    // #endif

    free(*item);
    *item = NULL; // opionated ptr erasiong: this avoids dangling ptrs.
}

/**
 * @brief Compares two itens.
 *
 * It is almost just a wrapper around strcmp. Keep in mind it does not check if
 * the pointers are null.
 *
 * @param item_1 ptr to an item.
 * @param item_2 ptr to an item.
 * @return int y. y = 0 for equal items. y < 0 when item_2 is ordered
 * alfabatically after item_1 and y > 0 otherwise.
 *
 */
int item_raw_cmp(const Item *item_1, const Item *item_2) {
    return strcmp(item_1->val.word, item_2->val.word);
}

/**
 * @brief Compares two itens.
 *
 * It is almost just a wrapper around strcmp; but it checks if the pointers are
 * null.
 *
 * @param item_1 ptr to an item.
 * @param item_2 ptr to an item.
 * @return int y. y = 0 for equal items. y < 0 when item_2 is ordered
 * alfabatically after item_1 and y > 0 otherwise.
 *
 */
int item_cmp(const Item *item_1, const Item *item_2) {
    if (NULL == item_1 || NULL == item_2) return 0;
    return item_raw_cmp(item_1, item_2);
}

// TODO: finish this func
void item_print(const Item *item) {
    item_print_word(item);
    printf(" ");
    item_print_description(item);
}

/**
 * @brief prints the word atribute found in the item entry.
 *
 * @param item ptr to the item.
 */
void item_print_word(const Item *item) {
#ifdef RELEASE
    printf("%s", item->val.word); // if release, trust and optimize
#else
    printf("%s", item ? item->val.word : "<NULL>");
#endif
}

/**
 * @brief prints the description atribute found in the item entry.
 *
 * @param item ptr to the item.
 */
void item_print_description(const Item *item) {
#ifdef RELEASE
    printf("%s", item->val.description); // if release, trust and optimize
#else
    printf("%s", item ? item->val.description : "<NULL>");
#endif
}

/**
 * @brief reads and constructs a item on the heap based on a user input from
 * stdin. It will read an input in the format `{W} {D}`, where W is a string
 * with no white space and D is a string that can include white spaces. A new
 * line "\n" will determine the end of the reading process.
 *
 * @return Item*
 *
 */
Item *item_read(void) {
    entry_t e = (entry_t){0};

    // TODO: proper error analysis and err handling 
    if (EOF == scanf("%50s %140[^\n]", e.word, e.description)) {
        return item_from_entry(e);
    } else {
        return item_from_entry(e);
    }

    // TODO: fix later

//     status_t flag = read_word(e.word);

//     // WARNING (b): if RECOVER_FROM_BIG_INPUT is not set to a truthy value, then
//     // you must deal with the error elsewhere or it will cause reading
//     // misbehaviour.
// #if RECOVER_FROM_BIG_INPUT
//     if (TOO_MUCH_ERR == flag) strutils_consume_word();
//     else if (EOF_ERR == flag) return NULL;
// #else
//     if (TOO_MUCH_ERR == flag || EOF_ERR == flag) return NULL;
// #endif

//     flag = read_description(e.description);

// #if RECOVER_FROM_BIG_INPUT
//     if (TOO_MUCH_ERR == flag) strutils_consume_line();
//     else if (EOF_ERR == flag) return NULL;
// #else
//     if (TOO_MUCH_ERR == flag || EOF_ERR == flag) return NULL;
// #endif

//     return item_from_entry(e);
}

Item *item_read_word(void) {
    entry_t e = (entry_t){0};
    status_t flag = read_word(e.word);

    // WARNING (b): if RECOVER_FROM_BIG_INPUT is not set to a truthy value, then
    // you must deal with the error elsewhere or it will cause reading
    // misbehaviour.
    // clang-format off
    #if RECOVER_FROM_BIG_INPUT
        if (TOO_MUCH_ERR == flag) strutils_consume_word();
        else if (EOF_ERR == flag) return NULL;
    #else
        if (TOO_MUCH_ERR == flag || EOF_ERR == flag) return NULL;
    #endif
    // clang-format on

    return item_from_entry(e);
}

int item_raw_char_cmp(const Item *item, const char c) {
    return item->val.word[0] - c;
}

//============================================================================//
//=================|    Private Function Implementations    |=================//
//============================================================================//

/**
 * @brief Creates a new item on the heap and returns a ptr to it.
 *
 * @param entry
 * @return Item*
 *
 * @warning: now you have ownership of the item.
 */
Item *item_from_entry(const entry_t entry) {
    Item *item = (Item *)malloc(sizeof(Item));
    if (NULL == item) printf ("COULD NOT ALLOC ITEM!\n");
    if (item) item->val = entry;
    return item;
}

/**
 * @brief Creates a item based on two strings.
 *
 * @param w a ptr to the string that will be the word with space allocated to,
 * at least MAX_WORD_SIZE bytes.
 * @param d a ptr to the string that will be the description with space
 * allocated to, at least MAX_DESCRIPTION_SIZE bytes.
 * @return Item* a pointer WITH OWNERSHIP to a new item.
 */
Item *item_from_strings(const char w[], const char d[]) {
    Item *item = (Item *)malloc(sizeof(Item));
    if (NULL == item) return NULL;
    memcpy(&item->val.word, w, MAX_WORD_SIZE);
    memcpy(&item->val.description, d, MAX_DESCRIPTION_SIZE);
    return item;
}

/**
 * @brief this will read a word, saving it to the s buffer. A word may not
 * containg any white space. Leading white spaces will be ignored.
 *
 * @param s output buffer, passed as argument.
 * @return status_t a code for the resulting status of the function execution.
 * It can only be `SUCCESS`, `EOF_ERR` or `TOO_MUCH_ERR`
 */
status_t read_word(char s[]) {
    // Ignoring leading white spaces
    int _flag = strutils_consume_spaces();
    if (EOF == _flag) {
        s[0] = '\0';
        return EOF_ERR;
    }

    // Reading
    int i;
    char c;
    for (i = 0;
         i < MAX_WORD_SIZE && EOF != (c = getchar()) && !strutils_isspace(c);
         i++) {
        s[i] = c;
    }

    // Ensuring proper string termination
    s[i] = '\0';

    // Outputing the status
    // if (EOF == c) return EOF_ERR;
    if (!strutils_isspace(c)) return TOO_MUCH_ERR;
    return SUCCESS;
}

/**
 * @brief this will read the description, saving it to the s buffer. The
 * description may containg white spaces. Leading and trailing white spaces will
 * be ignored/removed.
 *
 * @param s output buffer, passed as argument.
 * @return status_t a code for the resulting status of the function execution.
 * It can only be `SUCCESS`, `EOF_ERR` or `TOO_MUCH_ERR`
 */
status_t read_description(char s[]) {
    // Ignoring leading white spaces
    strutils_consume_spaces();

    // Reading
    int i;
    char c;
    for (i = 0; i < MAX_DESCRIPTION_SIZE && EOF != (c = getchar()) && '\n' != c;
         i++) {
        s[i] = c;
    }

    // Ensuring proper string termination
    s[i] = '\0';

    // Removing trailing white spaces
    i--;
    for (; 0 <= i; i--) {
        if (strutils_isspace(s[i])) s[i] = '\0';
        else break;
    }

    // Outputing the status
    if (EOF == c) return EOF_ERR;
    if ('\n' != c) return TOO_MUCH_ERR;
    return SUCCESS;
}

void item_raw_update(Item *destiny, const Item *reference) {
    destiny->val = reference->val;
}