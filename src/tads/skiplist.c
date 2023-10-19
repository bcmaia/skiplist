/**
 * @file skiplist.c
 * @brief Implementation of a Skip List data structure.
 */

#include "tads/skiplist.h"

//=============================================================================/
//=================|    Data Structures     |==================================/
//=============================================================================/

/**
 * @brief Node structure used in the Skip List.
 */
typedef struct _node_s {
    Item *item;
    struct _node_s *next;
    struct _node_s *down;
} Node;

/**
 * @brief Skip List data structure.
 */
struct _skiplist_s {
    Node *top;
    size_t length;
    size_t height;
};

//=============================================================================/
//=================|    Private Function Declarations     |====================/
//=============================================================================/

static Node *node_new(const Node base);
static void node_shallow_del(Node *node);
static void node_deep_del(Node *node);

static inline Node *mainlane_search(Node *sentinel, const Item *item);
static inline Node *fastlane_search(Node *sentinel, const Item *item);
static inline Node *express_search(Node *sentinel, const Item *item);

static status_t skiplist_raw_push_front(SkipList *skiplist, Item *item);
static _Bool skiplist_includes(const SkipList *skiplist, const Item *item);

static Node **skiplist_raw_trace(SkipList *skiplist, Item *item);
static Item *skiplist_raw_pop_front(SkipList *skiplist);

static _Bool valid_column(Node *n);

//=============================================================================/
//=================|    Public Function Implementations     |==================/
//=============================================================================/
SkipList *skiplist_new(void) {
    SkipList *skiplist = (SkipList *)malloc(sizeof(SkipList));
    if (skiplist) *skiplist = (SkipList){0};
    return skiplist;
}

void skiplist_del(SkipList **skiplist) {
    if (NULL == skiplist || NULL == (*skiplist)) return;

    // Freeing the fast lanes
    // NOTE (b): titanic will will sink, and the car will drive
    Node *titanic = (*skiplist)->top;
    while (NULL != titanic && NULL != titanic->down) { // loop throw the rows
        Node *temp_titanic = titanic->down;            // temp ptr
        Node *car = titanic;
        while (NULL != car) {           // loop throw the nodes in a row
            Node *temp_car = car->next; // temp ptr
            node_shallow_del(car);
            car = temp_car; // go to next
        }
        titanic = temp_titanic; // go to bellow
    }

    // Freeing the main lane
    // NOTE: now, titanic reachs the botom and slides across the barren sea
    // botom,
    //  the end is near.
    while (NULL != titanic) {               // loop throw the nodes in a row
        Node *temp_titanic = titanic->next; // temp ptr
        node_deep_del(titanic);
        titanic = temp_titanic; // go to bellow
    }

    free(*skiplist);
    *skiplist = NULL;
}

status_t skiplist_insert(SkipList *skiplist, Item *item) {
    // PART 1: Basic checks and trivial cases
    // Error handling
    if (NULL == skiplist || NULL == item) return NUL_ERR;
    if (skiplist_includes(skiplist, item)) return REPEATED_ENTRY_ERR;
    if (skiplist_is_full(skiplist)) return ARR_IS_FULL_ERR;

    // Trivial case: the list is empty
    if (skiplist_is_empty(skiplist)) {
        skiplist->top = node_new((Node){.item = item});

        skiplist->length = 1;
        skiplist->height = 1;

        return SUCCESS;
    }

    // Trivial case: insertion at the begginig
    if (item_cmp(skiplist->top->item, item) >= 0) {
        return skiplist_raw_push_front(skiplist, item);
    }

    // PART 2: Tracing the skiplist
    // Getting the update trace
    Node **updates = skiplist_raw_trace(skiplist, item); // ownership
    if (NULL == updates) return NUL_ERR;                 // err handling
    size_t h = skiplist->height; // mem access optimization

    if (NULL == updates[h - 1]) { // err handling
        free(updates);
        return NUL_ERR;
    }

    // PART 3: Insertion at the main lane
    // Creating the new node
    Node *new_node = node_new((Node){
        .item = item,
        .next = updates[h - 1]->next,
    });

    // Error handling
    if (NULL == new_node) {
        free(updates);
        return ALLOC_ERR;
    }

    // Inserting the new node
    updates[h - 1]->next = new_node;

    // PART 4: Insertion at the existing fast lanes
    long long lv = h - 2;
    for (; (lv >= 0) && (rand() & 1); lv--) {
        // TODO: Add error handling here
        new_node = node_new((Node){
            .item = item,
            .next = updates[lv]->next,
            .down = new_node,
        });
        if (new_node) updates[lv]->next = new_node;
        else break; // err handling: recoverable error
        // NOTE: no need for a more complex error handling here. A fail here
        // only stops us from clibimbing a level. Thats fine.
    }

    // Raising the level of the node above the current level of the skiplist
    if (lv < 0 && new_node) {
        lv = h;
        for (; (rand() & 1);) {
// clang-formater off
#ifdef SKIPLIST_MAX_HEIGHT
            if (lv++ >= SKIPLIST_MAX_HEIGHT) break;
#endif
            // clang-formater on

            // New node
            new_node = node_new((Node){
                .item = item,
                .down = new_node,
            });

            // Error handling
            if (NULL == new_node) break;

            // New top
            Node *temp = node_new((Node){
                .item = skiplist->top->item,
                .next = new_node,
                .down = skiplist->top,
            });

            // Err handling
            if (NULL == temp) {
                node_shallow_del(new_node);
                break;
            }

            skiplist->top = temp;
            skiplist->height++;
        }
    }

    // PART 6: cleanup and finishing it
    skiplist->length++;
    free(updates);
    return SUCCESS;
}

_Bool skiplist_debug_validate(const SkipList *skiplist) {
    if (NULL == skiplist) return 0;
    if (skiplist->length > 0 && NULL == skiplist->top) return 0;

    size_t depth = 0;
    size_t len = 0;

    Node *sentinel = skiplist->top;
    if (sentinel)
        while (sentinel->down) {
            while (sentinel->next) {
                if (!valid_column(sentinel)) return 0;
                sentinel = sentinel->next;
                len++;
            }
            sentinel = sentinel->down;
            depth++;
        }

    // if ((1 + depth) != skiplist->height) return 0;
    // if ((1 + len) != skiplist->height) return 0;

    return 1;
}

void skiplist_debug_print(const SkipList *skiplist) {
    if (NULL == skiplist) { // err handling
        printf("ERROR: skiplist ptr is NULL. \n");
        return;
    }

    int lv = skiplist->height;

    // Loop throw all the lanes
    for (Node *digger = skiplist->top; NULL != digger; digger = digger->down) {
        printf("lv %d) ", lv); // print level index
        lv -= 1;

        // Loop throw a lane
        for (Node *runner = digger; NULL != runner; runner = runner->next) {
            printf("[");
            item_print(runner->item);
            printf("]; ");
        }

        printf(".\n\n\n");
    }
}

Item *skiplist_search(const SkipList *skiplist, const Item *item) {
    // Err handling when null poiter
    // WARNING: other erros such as invalid ptr will not be caught.
    if (NULL == skiplist || NULL == item) return NULL;

    // Declarations and initializations
    Node *sentinel = skiplist->top;

    // Trivial case: empty list
    if (NULL == sentinel) return NULL;

    // Search in fast lanes
    sentinel = express_search(sentinel, item);

    // Search in main lane
    sentinel = mainlane_search(sentinel, item);

    _Bool found_it = sentinel && 0 == item_cmp(sentinel->item, item);
    return found_it ? sentinel->item : NULL;
}

status_t skiplist_update(SkipList *skiplist, Item *item) {
    // Err handling
    if (NULL == skiplist || NULL == item) return NUL_ERR;
    if (skiplist_is_empty(skiplist) || !skiplist_includes(skiplist, item))
        return NOT_FOUND_ERR;

    // Trivial case: update the first item
    if (0 == item_cmp(skiplist->top->item, item)) {
        Node *sentinel = skiplist->top;
        for (; sentinel; sentinel = sentinel->down)
            item_raw_update(sentinel->item, item);
        return SUCCESS;
    }

    // Shortcuts
    size_t h = skiplist->height;

    // Get the node trace.
    Node **trace = skiplist_raw_trace(skiplist, item); // ownership
    if (NULL == trace) return ALLOC_ERR;               // err handling

    if (NULL == trace[h - 1]) { // err handling
        free(trace);
        return ERROR; // err handling
    }

    // Updating
    Item *item_ptr = trace[h - 1]->next->item;
    for (long long i = h - 1; i >= 0; i--) {
        // If we find a next in trace that does not point to a clone of the
        // target item, halt.
        if (NULL == trace[i]->next || item_ptr != trace[i]->next->item) break;
        else item_raw_update(trace[i]->next->item, item); // update
    }

    // Cleanup and exit
    free(trace);
    return SUCCESS;
}

Item *skiplist_remove(SkipList *skiplist, Item *item) {
    // Err handling
    if (NULL == skiplist || NULL == item) return NULL;
    if (skiplist_is_empty(skiplist) || !skiplist_includes(skiplist, item)) {
        return NULL;
    }

    // Trivial case: remove first item
    if (0 == item_cmp(skiplist->top->item, item)) {
        return skiplist_raw_pop_front(skiplist);
    }

    // Shortcuts
    size_t h = skiplist->height;

    // Getting node trace
    Node **updates = skiplist_raw_trace(skiplist, item);
    if (NULL == updates) return NULL; // err handling

    // Error handling
    if (NULL == updates[h - 1] || NULL == updates[h - 1]->next ||
        NULL == updates[h - 1]->next->item) {
        free(updates);
        return NULL;
    }

    // Saving the result
    Item *result = updates[h - 1]->next->item;

    // Removing
    long long i = skiplist->height - 1;
    for (; i >= 0 && updates[i] && updates[i]->next &&
           result == updates[i]->next->item;
         i--) {
        if (NULL == updates[i] || NULL == updates[i]->next) continue;

        Node *temp = updates[i]->next->next;

        *(updates[i]->next) = (Node){0};
        node_shallow_del(updates[i]->next);
        updates[i]->next = NULL;

        updates[i]->next = temp;
    }

    // Reduce level
    if (NULL == skiplist->top->next && skiplist->height > 1) {
        Node *temp = skiplist->top->down;
        node_shallow_del(skiplist->top);
        skiplist->top = temp;
        skiplist->height--;
    }

    // Cleanup and exit
    free(updates);
    skiplist->length--;
    return result;
}

status_t skiplist_print(const SkipList *skiplist, const char c) {
    if (NULL == skiplist || skiplist_is_empty(skiplist)) return ERROR;

    Node *sentinel = skiplist->top;

    if (sentinel)
        while (sentinel->down) {
            while (sentinel->next &&
                   item_raw_char_cmp(sentinel->next->item, c) < 0)
                sentinel = sentinel->next;
            sentinel = sentinel->down;
        }

    while (sentinel && item_raw_char_cmp(sentinel->item, c) < 0)
        sentinel = sentinel->next;

    while (sentinel && item_raw_char_cmp(sentinel->item, c) == 0) {
        item_print(sentinel->item);
        printf("\n");
        sentinel = sentinel->next;
    }

    // return 0 == count ? ERROR : SUCCESS; // NOTE: is this right?
    return SUCCESS;
}

// Temporary disable the "-Wunused-parameter" warning.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

_Bool skiplist_is_full(const SkipList *skiplist) {
    // clang-format off
    #ifdef SKIPLIST_MAX_LENGTH
        return skiplist ? SKIPLIST_MAX_LENGTH <= skiplist->length : 0;
    #else
        return 0;
    #endif // SKIPLIST_MAX_LENGTH
    // clang-format on
}

// restoring the diagnostic stack to its previous state.
#pragma GCC diagnostic pop

_Bool skiplist_is_empty(const SkipList *skiplist) {
    return skiplist ? skiplist->length <= 0 : 0;
}

size_t skiplist_length(const SkipList *skiplist) {
    return skiplist ? skiplist->length : 0;
}

size_t skiplist_height(const SkipList *skiplist) {
    return skiplist ? skiplist->height : 0;
}

//=============================================================================/
//=================|    Private Function Implementations    |==================/
//=============================================================================/

/**
 * @brief Creates a heap allocated skiplist node based on a stack allocated skip
 * list.
 *
 * @param base a stack allocated node that will be copied into the heap.
 * @return Node* ptr to the heap allocated node, WITH OWNERSHIP, or NULL on
 * error.
 */
static Node *node_new(const Node base) {
    Node *n = (Node *)malloc(sizeof(Node)); // mem alloc
    if (n) *n = base;                       // if no error, init
    return n;                               // return ptr, might be null
}

/**
 * @brief frees the memory used to store a node on the heap without freeing
 the
 *  memory used to store the node contents.
 * @warning if item is not freed elsewhere, this function will cause a memory
 *  leak.
 *
 * @param node
 */
static void node_shallow_del(Node *node) { free(node); }

/**
 * @brief frees the memory used to store a node and its contetns.
 * @warning if item is used elsewhere after a node containing a reference
 *  to it if deleted using this function, a segfault by dangling ptr access may
 *  ocur.
 *  leak.
 *
 * @param node
 */
static void node_deep_del(Node *node) {
    if (node) item_del(&node->item);
    free(node);
}

_Bool skiplist_includes(const SkipList *skiplist, const Item *item) {
    Item *it = skiplist_search(skiplist, item);
    return it && 0 == item_cmp(it, item) ? 1 : 0;
}

static inline Node *mainlane_search(Node *sentinel, const Item *item) {
    // if (NULL == sentinel) return NULL;
    while (sentinel && item_cmp(sentinel->item, item) < 0) {
        sentinel = sentinel->next;
    }
    return sentinel;
}

static inline Node *fastlane_search(Node *sentinel, const Item *item) {
    // WARNING: We assume `NULL != sentinel` and `NULL != item`
    if (NULL == sentinel) return NULL;
    while (sentinel->next && item_cmp(sentinel->next->item, item) < 0) {
        sentinel = sentinel->next;
    }
    return sentinel;
}

static inline Node *express_search(Node *sentinel, const Item *item) {
    // WARNING: We assume `NULL != sentinel` and `NULL != item`
    if (NULL == sentinel) return NULL;
    while (sentinel->down) { // while sentinel.level > 0
        while (sentinel->next && item_cmp(sentinel->next->item, item) < 0) {
            sentinel = sentinel->next;
        }
        sentinel = sentinel->down;
    }
    return sentinel;
}

/**
 * @brief Searched the skiplist and returns every node, one for each level, that
 * preceeds a node equal o grater than the target item.
 *
 * @note this function assumes that skiplist and item are valid pointers and it
 * also assumes that the list is not empty.
 *
 * @param skiplist a ptr to the skiplist.
 * @param item
 * @return Node**
 */
Node **skiplist_raw_trace(SkipList *skiplist, Item *item) {
    if (NULL == skiplist || item == NULL || skiplist->top == NULL) return NULL;

    Node **updates = (Node **)calloc(skiplist->height, sizeof(Node *));
    if (NULL == updates) return NULL;

    size_t depth = 0;
    Node *sentinel = skiplist->top;

    if (sentinel)
        while (sentinel->down) { // while sentinel.level > 0
            while (sentinel->next && item_cmp(sentinel->next->item, item) < 0) {
                sentinel = sentinel->next;
            }
            updates[depth++] = sentinel;
            sentinel = sentinel->down;
        }

    if (sentinel)
        while (sentinel->next && item_cmp(sentinel->next->item, item) < 0) {
            sentinel = sentinel->next;
        }

    updates[depth++] = sentinel;
    // printf("<trace depth(%lu)>\n", depth);

    return updates;
}

/**
 * @brief Inserts a item on the start of the skiplist.
 * 
 * @param skiplist ptr to the skiplist.
 * @param item ptr to the item that will be inserted. You lose ownership on a successiful push.
 * @return status_t \c SUCCESS if we succeded 
 */
static status_t skiplist_raw_push_front(SkipList *skiplist, Item *item) {
    // New top
    Node *new_node = node_new((Node){.item = item, .next = skiplist->top});
    if (NULL == new_node) return NUL_ERR; // Error handling
    Node *sentinel = skiplist->top->down;
    skiplist->top = new_node;

    // Creating the new nodes for each level
    Node *previous = new_node;
    while (sentinel) {
        // Creating the new node
        new_node = node_new((Node){.item = item, .next = sentinel});

        // Error handling: revert all to recover
        if (NULL == new_node) {
            Node *previous_top = skiplist->top->next;

            sentinel = skiplist->top; // current top
            while (sentinel) {
                Node *temp = sentinel->down;
                node_shallow_del(sentinel);
                sentinel = temp;
            }

            skiplist->top = previous_top;
        }

        // Adding the new node
        previous->down = new_node;
        previous = new_node;

        sentinel = sentinel->down;
    }

    skiplist->length++;
    return SUCCESS;
}

/**
 * @brief Reduces the height of the skiplist if it has small lanes on the top.
 *
 * Every lane above the main lane (the bottom lane, aka: the true lane), if they
 * exist, should have at least 2 itens. Otherwise, there would be no peformance
 * gain provided by the lane ;)
 *
 * @param skiplist a pointer to the skiplist.
 */
void skiplist_trim(SkipList *skiplist) {
    if (NULL == skiplist || NULL == skiplist->top) return; // err handling

    // Loop from top to bottom untill we reach the bottom or until a valid top
    // lane is stabilished.
    while (skiplist->height > 1 && NULL == skiplist->top->next) {
        // Destroy the lane
        Node *temp = skiplist->top->down;
        node_shallow_del(skiplist->top);
        skiplist->top = temp;

        skiplist->height--; // dec the height
    }
}

/**
 * @brief removes the first element of the skiplist.
 * 
 * @param skiplist ptr to skiplist.
 * @return Item* ptr to the the removed item or NULL. You have ownership.
 */
Item *skiplist_raw_pop_front(SkipList *skiplist) {
    Item *return_item = skiplist->top->item;
    Node *sentinel = skiplist->top;

    // Trivial case: list has only one
    if (1 == skiplist->length) {
        // Cleanup loop
        while (sentinel) {
            Node *temp = sentinel->down;
            node_shallow_del(sentinel);
            sentinel = temp;
        }

        // Set zeros
        *skiplist = (SkipList){0};

        return return_item;
    }

    // Tracing next
    size_t h = skiplist->height;
    Node **updates = (Node **)calloc(h, sizeof(Node *));
    if (NULL == updates) return NULL; // err handling

    // Going down and tracing
    size_t depth = 0;
    while (sentinel) {
        updates[depth++] = sentinel->next;
        sentinel = sentinel->down;
    }

    // The new start of the main lane (bottom lane):
    Node *new_bottom = updates[h - 1];

    if (NULL == new_bottom) { // err handling
        free(updates);
        return NULL;
    }

    Item *bottom_item = new_bottom->item;

    // Making shure the first object is tall enought
    Node *temp = updates[h - 1];
    long long i = h - 2;
    for (; i >= 0 && updates[i]->item == bottom_item; i--)
        temp = updates[i];

    // Raise level of the new first object if needed
    // NOTE (b): `i` can only be inside [-1, h - 2] here.
    for (; i >= 0; i--) {
        temp = node_new((Node){
            .item = bottom_item,
            .next = updates[i],
            .down = temp,
        });
    }

    // Setting new top
    sentinel = skiplist->top;
    skiplist->top = temp;

    // Deleting the old first
    while (sentinel) {
        temp = sentinel->down;
        node_shallow_del(temp);
        sentinel = temp;
    }

    // Trimming the skiplist: it might be taller than needed.
    skiplist_trim(skiplist);

    // Cleanup and exit
    skiplist->length--;
    free(updates);
    return return_item;
}

/**
 * @brief Checks if all the nodes in a column is are valid.
 *
 * @param n a pointer to the node.
 * @return _Bool \c 1 if the column is valid, \c 0 if it contains one o more
 * detectable errors.
 */
static _Bool valid_column(Node *n) {
    Node *sentinel = n;
    if (sentinel)
        while (sentinel->down) {
            if (sentinel->down->item != sentinel->item) return 0;
            sentinel = sentinel->down;
        }
    return 1;
}