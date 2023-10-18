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
 * @brief Node structure used in a linked list stack for tracing nodes in
 * levels.
 */
typedef struct _linked_stack_node_s {
    Node *n;
    struct _linked_stack_node_s *next;
} LinkedStackNode;

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
/**
 * @brief Creates a heap allocated skiplist node based on a stack allocated skip
 * list.
 *
 * @param base a stack allocated node that will be copied into the heap.
 * @return Node* ptr to the heap allocated node, WITH OWNERSHIP, or NULL on
 * error.
 */
static Node *node_new(const Node base);

/**
 * @brief frees the memory used to store a node on the heap without freeing
 the
 *  memory used to store the node contents.
 * @warning if item is not freed elsewhere, this function will cause a memory
 *  leak.
 *
 * @param node
 */
static void node_shallow_del(Node *node);

/**
 * @brief frees the memory used to store a node and its contetns.
 * @warning if item is used elsewhere after a node containing a reference
 *  to it if deleted using this function, a segfault by dangling ptr access may
 *  ocur.
 *  leak.
 *
 * @param node
 */
static void node_deep_del(Node *node);

/**
 * @brief Creates a linked list stack node.
 *
 * @param node ptr which will be stored in this linked list node.
 * @param next ptr to the next linked list node.
 * @return LinkedStackNode* ptr to the new LinkedStackNode, WITH OWNERSHIP, or
 * NULL on error.
 */
// static LinkedStackNode *
// linked_stack_node_new(Node *node, LinkedStackNode *next);

static inline Node *mainlane_search(Node *sentinel, const Item *item);
static inline Node *fastlane_search(Node *sentinel, const Item *item);
static inline Node *express_search(Node *sentinel, const Item *item);
static status_t skiplist_raw_push_front(SkipList *skiplist, Item *item);
_Bool skiplist_includes(const SkipList *skiplist, const Item *item);
Node *skiplist_node_search(const SkipList *skiplist, const Item *item);
Node **skiplist_raw_trace(SkipList *skiplist, Item *item);
Item *skiplist_raw_pop_front(SkipList *skiplist);

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

static inline size_t skiplist_random_levels(void) {
    // clang-format off
    #ifdef SKIPLIST_MAX_HEIGHT
        return 1 + geometric_dist_test(SKIPLIST_PROB, SKIPLIST_MAX_HEIGHT - 1);
    #else
        const int MAX_VALUE = 1000000000;
        return 1 + geometric_dist_test(SKIPLIST_PROB, MAX_VALUE); 
    #endif
}

// void skiplist_raise (SkipList *skiplist, Item *item);
status_t skiplist_insert(SkipList *skiplist, Item *item) {
    // PART 1: Basic checks and trivial cases
    // Error handling
    if (NULL == skiplist || NULL == item) return NUL_ERR;
    if (skiplist_includes(skiplist, item)) return REPEATED_ENTRY_ERR;
    if (skiplist_is_full(skiplist)) return ARR_IS_FULL_ERR;

    // Trivial case: the list is empty
    if (skiplist_is_empty(skiplist)) {
        // printf("Insertion: empty list!\n");
        skiplist->top = node_new((Node){.item = item});

        skiplist->length = 1;
        skiplist->height = 1;

        return SUCCESS;
    }

    // Trivial case: insertion at the begginig
    if (item_cmp(skiplist->top->item, item) >= 0) {
        // printf("Insertion: on the start!\n");
        return skiplist_raw_push_front(skiplist, item);
    }

    // PART 2: Tracing the skiplist
    // Getting the update trace
    Node **updates = skiplist_raw_trace(skiplist, item); // ownership
    size_t h = skiplist->height;
    if (NULL == updates) return NUL_ERR;
    if (NULL == updates[h - 1]) {
        // printf("TRACE HAS NULL!\n");
        free(updates);
    }

    // PART 3: Insertion at the main lane
    // mem access optimization
    

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
    // Randomily deciding at how many levels the node will rezide
    size_t levels = skiplist_random_levels();

    // NOTE: since we can garantee that at least one node was already inserted,
    // we subtract 2 to get the correct index.
    long long lv = h - 2;
    long long limit = levels < h ? h - levels : 0; 
    // printf("<insertion levels(%lu) h(%lu)\n", levels, h);

    // Adding randomily new levels on the fast lanes
    for (; lv >= limit; lv--) {
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

    // PART 5: Raising the skiplist current level
    // clang-format off
    #if RAISE_ONLY_ONCE
        lv = h < levels; // lv will be 0 or 1
    #else
        lv = levels - h;
    #endif
    // clang-format on

    // If no errors occurred and lv is greater than 0, proceed and
    // Raise level
    if (new_node) {
        for (; lv > 0; lv--) {
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

/**
 * @brief Searched for an item in the skiplist.
 *
 * @param skiplist ptr to the skiplist.
 * @param item ptr to the item.
 * @return Item* NULL if nothing is found or, if founded, a ptr to the item
 * WITHOUT OWNERSHIP.
 */
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
    // Error handling
    if (NULL == skiplist || NULL == item) return NUL_ERR;
    // clang-format off
    if (
        skiplist_is_empty(skiplist) 
        || !skiplist_includes(skiplist, item)
    )  {
        if (skiplist_is_empty(skiplist) ) return NOT_FOUND_ERR;

        printf("\n");
        printf("NOT FOUND! [");
        item_print(item);
        printf("]; \n");
        printf("FOUND: [");
        item_print(skiplist_search(skiplist, item));
        printf("]; \n");
        // printf("LIST:\n");
        // skiplist_debug_print(skiplist);
        // printf("\n\n");
        exit(1);

        return NOT_FOUND_ERR;
    }
    // clang-format on

    // Trivial case: update the first item
    if (0 == item_cmp(skiplist->top->item, item)) {
        Node *sentinel = skiplist->top;
        for (; sentinel; sentinel = sentinel->down)
            item_raw_update(sentinel->item, item);
        return SUCCESS;
    }

    // Trace
    Node **updates = skiplist_raw_trace(skiplist, item); // ownership
    if (NULL == updates) return ERROR;                   // err handling

    // Updating
    size_t h = skiplist->height;
    for (size_t i = 0; i < h; i++)
        if (updates[i] && updates[i]->next && updates[i]->next->item)
            item_raw_update(updates[i]->next->item, item);

    // Cleanup and exit
    free(updates);
    return SUCCESS;
}

Item *skiplist_remove(SkipList *skiplist, Item *item) {
    // clang-format off
    if (
        NULL == skiplist
        || NULL == item
        || !skiplist_includes(skiplist, item)
        || skiplist_is_empty(skiplist)
    ) return NULL;
    // clang-format on

    // Trivial case: remove first item
    if (0 == item_cmp(skiplist->top->item, item))
        return skiplist_raw_pop_front(skiplist);

    Node *sentinel = skiplist->top;
    if (NULL == sentinel) return NULL;

    Node **updates = skiplist_raw_trace(skiplist, item);
    if (NULL == updates) return NULL;
    if (NULL == updates[skiplist->height - 1] ||
        NULL == updates[skiplist->height - 1]->next ||
        NULL == updates[skiplist->height - 1]->next->item) {
        free(updates);
        return NULL;
    }
    Item *result = updates[skiplist->height - 1]->next->item;

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

    free(updates);
    skiplist->length--;
    return result;
}

status_t skiplist_print(const SkipList *skiplist, const char c) {
    if (NULL == skiplist || skiplist_is_empty(skiplist)) return ERROR;

    Node *sentinel = skiplist->top;

    while (sentinel->down) {
        while (sentinel && item_raw_char_cmp(sentinel->item, c) < 0)
            sentinel = sentinel->next;
        if (NULL == sentinel) break;
        sentinel = sentinel->down;
    }

    while (sentinel && item_raw_char_cmp(sentinel->item, c) < 0)
        sentinel = sentinel->next;

    // size_t count = 0;
    while (sentinel && item_raw_char_cmp(sentinel->item, c) == 0) {
        item_print(sentinel->item);
        printf("\n");
        sentinel = sentinel->next;
        // count++;
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

static Node *node_new(const Node base) {
    Node *n = (Node *)malloc(sizeof(Node)); // mem alloc
    if (n) *n = base;                       // if no error, init
    return n;                               // return ptr, might be null
}

static void node_shallow_del(Node *node) { free(node); }

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
    // if (NULL == sentinel) return NULL;
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
 * @brief
 *
 * @note this function assumes that skiplist and item are valid pointers and it
 * also assumes that the list is not empty.
 *
 * @param skiplist
 * @param item
 * @return Node**
 */
Node **skiplist_raw_trace(SkipList *skiplist, Item *item) {
    if (NULL == skiplist || item == NULL || skiplist->top == NULL) return NULL;

    Node **updates = (Node **)calloc(1 + skiplist->height, sizeof(Node*));
    if (NULL == updates) return NULL;

    size_t depth = 0;
    Node *sentinel = skiplist->top;
    
    while (sentinel->down) { // while sentinel.level > 0
        while (sentinel->next && item_cmp(sentinel->next->item, item) < 0) {
            sentinel = sentinel->next;
        }
        updates[depth] = sentinel;
        sentinel = sentinel->down;
        depth++;
    }

    while (sentinel->next && item_cmp(sentinel->next->item, item) < 0) {
        sentinel = sentinel->next;
    }
    updates[depth] = sentinel;
    // printf("<trace depth(%lu)>\n", depth);

    return updates;
}

static status_t skiplist_raw_push_front(SkipList *skiplist, Item *item) {
    // New top
    Node *new_node = node_new((Node){.item = item, .next = skiplist->top});
    if (NULL == new_node) return NUL_ERR; // Error handling
    Node *sentinel = skiplist->top->down;
    skiplist->top = new_node;

    // Creating the new nodes for each level
    Node *previous = new_node;
    for (; sentinel; sentinel = sentinel->down) {
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
    }

    // long long levels = ((long long)skiplist_random_levels()) - ((long
    // long)skiplist->height); for (; levels > 0; levels--) {

    // }

    skiplist->length++;
    return SUCCESS;
}

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
    Node **updates = (Node **)malloc(h * sizeof(Node *));
    size_t depth = 0;

    for (; sentinel; sentinel = sentinel->down, depth++)
        updates[depth] = sentinel->next;

    Node *new_bottom = updates[h - 1];
    Item *bottom_item = new_bottom->item;

    // Making shure the first object is tall enought
    long long i = h - 2;
    for (; i >= 0 && updates[i]->item == bottom_item; i--)
        ;

    // Raise level of the new first object if needed
    // NOTE (b): `i` can only be inside [-1, h - 2] here.
    Node *temp = updates[i + 1];
    for (; i >= 0 && updates[i]->next; i--) {
        temp = node_new((Node){
            .item = bottom_item,
            .next = updates[i]->next,
            .down = temp,
        });
    }

    // Delete excess levels
    for (; i >= 0; i--) {
        node_shallow_del(updates[i]);
        skiplist->height--;
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

    skiplist->length--;
    return return_item;
}