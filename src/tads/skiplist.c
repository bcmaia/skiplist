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

// static inline void undo_insert_at_start(SkipList *sl);

Node **skiplist_search_n_trace(SkipList *skiplist, Item *item);
// static status_t skiplist_push_front(SkipList *skiplist, Item *item);
_Bool skiplist_includes(const SkipList *skiplist, const Item *item);
Node *skiplist_node_search(const SkipList *skiplist, const Item *item);

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

// status_t skiplist_insert(SkipList *skiplist, Item *item) {
//     // Err handling when null poiter
//     // WARNING: other erros such as invalid ptr will not be caught.
//     if (NULL == skiplist || NULL == item) return NUL_ERR;
// #ifdef SKIPLIST_MAX_LENGTH
//     if (skiplist_is_full(skiplist)) return ARR_IS_FULL_ERR;
// #endif

//     // Trivial case: empty list
//     if (skiplist_is_empty(skiplist)) {
//         skiplist->top = node_new((Node){.item = item});
//         if (NULL == skiplist->top) return ALLOC_ERR;

//         skiplist->height = 1;
//         skiplist->length = 1;

//         return SUCCESS;
//     }

//     // Trivial case: repeated at start
//     if (0 == item_cmp(skiplist->top->item, item)) {
//         return REPEATED_ENTRY_ERR;
//     }

//     // Trivial case: Insersion at start:
//     if (item_cmp(skiplist->top->item, item) > 0) {
//         return skiplist_push_front(skiplist, item);
//     }

//     // Trace searching
//     size_t height = skiplist->height;
//     Node **trace = skiplist_search_n_trace(skiplist, item); // ownership
//     if (NULL == trace) return ALLOC_ERR;

//     // Repeated entry
//     if (0 == item_cmp(trace[height - 1]->item, item)) {
//         free(trace);
//         return REPEATED_ENTRY_ERR;
//     }

//     // Inserting at every level
//     Node *new_node = NULL;
//     long long i = height - 1;
//     for (; 0 >= i; i--) {
//         new_node = node_new((Node
//         ){.item = item, .next = trace[i]->next, .down = new_node});
//         if (NULL == new_node) return PARTIAL_FAILURE;
//         trace[i]->next = new_node;
//         if (rand() & 1) break;
//     }

//     // Raise level
//     if ((0 < i) && (rand() & 1)) {
//         skiplist->top =
//             node_new((Node){.item = item, .next = NULL, .down = trace[0]});
//         skiplist->height++;
//     }

//     skiplist->length++; // inc length
//     free(trace);        // free memory

//     return SUCCESS;
// }

status_t skiplist_insert(SkipList *skiplist, Item *item) {
    if (skiplist_includes(skiplist, item)) return REPEATED_ENTRY_ERR;

    if (skiplist_is_empty(skiplist)) {
        skiplist->top = node_new((Node){.item = item});
        skiplist->top = node_new((Node){.item = item, .down = skiplist->top});

        skiplist->length = 1;
        skiplist->height = 2;

        return SUCCESS;
    }

    Node *sentinel = skiplist->top;

    size_t h = skiplist->height;
    Node **updates = (Node **)malloc(h * sizeof(Node *));

    size_t depth = 0;
    while (sentinel->down != NULL) {
        while (sentinel->next && item_cmp(sentinel->next->item, item) < 0) {
            sentinel = sentinel->next;
        }
        updates[depth++] = sentinel;
        sentinel = sentinel->down;
    }

    while (sentinel->next && item_cmp(sentinel->next->item, item) < 0) {
        sentinel = sentinel->next;
    }
    updates[depth++] = sentinel;

    // Inserção
    size_t curr_lv = 0;
    Node *new_node;
    while (curr_lv < skiplist->height) {
        new_node = node_new((Node){.item = item});
        if (0 == curr_lv) {
            new_node->next = updates[curr_lv]->next;
            new_node->down = NULL; // NOTE: unessessary // TODO: remove
        } else {
            new_node->next = updates[curr_lv]->next;
            new_node->down = updates[curr_lv - 1]->next;
        }
        updates[curr_lv]->next = new_node;
        curr_lv++;
    }

    while (rand() & 1) {
        new_node = node_new((Node){.item = item, .down = new_node});
        skiplist->top = node_new((Node
        ){.item = item, .next = new_node, .down = skiplist->top});
        skiplist->height++;
    }

    skiplist->length++;
    free(updates);
    return SUCCESS;
}

void skiplist_debug_print(const SkipList *skiplist) {
    if (NULL == skiplist) { // err handling
        printf("ERROR: skiplist ptr is NULL. \n");
        return;
    }

    int lv = skiplist->height - 1;

    // Loop throw all the lanes
    for (Node *digger = skiplist->top; NULL != digger; digger = digger->down) {
        printf("lv %d) ", lv--); // print level index

        // Loop throw a lane
        for (Node *runner = digger; NULL != runner; runner = runner->next) {
            printf("[");
            item_print(runner->item);
            printf("]; ");
        }

        printf("\n");
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

// void skiplist_strip(SkipList *skiplist) {}

// Item *skiplist_pop_front(SkipList *skiplist, const Item *item) {
//     if (1 == skiplist->length) {
//         // Freeing every node on the fast lanes
//         Node *sentinel = skiplist->top;
//         for (; sentinel->down; sentinel = sentinel->down) {
//             node_shallow_del(sentinel);
//         }

//         // Final node
//         Item *it = sentinel->item;
//         node_shallow_del(sentinel);

//         // Nullify everything
//         *skiplist = (SkipList){0};

//         return it;
//     }

//     // Tracing the nodes
//     size_t h = skiplist->height;
//     Item *it = NULL;
//     Node *arr[h];
//     long long i = 0;
//     Node *sentinel = skiplist->top;

//     for (; sentinel; sentinel = sentinel->down) {
//         arr[i++] = sentinel;
//     }

//     // Geting a ptr to the next item
//     i--; // Now i is the last index
//     it = arr[i]->next->item;
//     i--; // now i is the pre last index

//     // clang-format off
//     // This item will become the next top, so we might need to create new
//     nodes.
//     // Ignoring the nodes above the item
//     for (; it == arr[i]->item && 0 <= i; i--);

//     // Creating new nodes
//     Node* new_node = 0 > i ? NULL : arr[i + 1];
//     for (; 0 <= i; i--) {
//         Node* temp = new_node;
//         new_node = node_new((Node){
//             .item = it,
//             .next = arr[i]->next,
//             .down = new_node
//         });

//         // Error handling: recover
//         if (NULL == new_node) {
//             Node* sentinel = temp;
//             while (sentinel->down) {
//                 Node* temp = sentinel->down;
//                 node_shallow_del(sentinel);
//                 sentinel = temp;
//             }
//             return NULL;
//         }
//     }

//     // Deleting nodes
//     sentinel = skiplist->top;
//     it = arr[h - 1]->item;
//     skiplist->top = new_node; // new top
//     for (i = 0; i < (long long)h; i++) {
//         node_shallow_del(arr[i]);
//     }

//     return it;
// }

Item *skiplist_remove(SkipList *skiplist, const Item *item) {
    if (!skiplist_includes(skiplist, item)) return NULL;

    Node *sentinel = skiplist->top;
    if (NULL == sentinel) return NULL;

    size_t h = skiplist->height;
    Node *updates[h];

    size_t depth = 0;
    while (sentinel && sentinel->down != NULL) {
        while (sentinel && item_cmp(sentinel->next->item, item) < 0) {
            sentinel = sentinel->next;
        }
        updates[depth++] = sentinel;
        sentinel = sentinel->down;
    }

    while (sentinel && item_cmp(sentinel->next->item, item) < 0) {
        sentinel = sentinel->next;
    }
    updates[depth++] = sentinel;
    Item *result = sentinel->item;

    size_t curr_lv = 0;
    while (curr_lv < skiplist->height) {
        if (0 == item_cmp(updates[curr_lv]->next->item, item)) {
            Node *rem = updates[curr_lv]->next;
            updates[curr_lv]->next = rem->next;
            node_shallow_del(rem);
        }
        curr_lv++;
    }

    // NOTE: this might give an error if the list is to short
    while (skiplist->top->next == NULL) {
        Node *rem = skiplist->top;
        skiplist->top = skiplist->top->down;
        skiplist->height--;
        node_shallow_del(rem); // possible leak here
    }

    return result;
}

void skiplist_print(const SkipList *skiplist, const char c) {
    if (NULL == skiplist) {
        printf("Error: cannot print since skiplist ptr is null.");
        return;
    }
    if (skiplist_is_empty(skiplist)) {
        printf("Error: cannot print since skiplist is empty.");
        return;
    }

    Node *sentinel = skiplist->top;
    while (sentinel && sentinel->down && item_char_cmp(sentinel->item, c) <= 0) {
        while (sentinel && item_char_cmp(sentinel->item, c) < 0) {
            sentinel = sentinel->next;
        }
        sentinel = sentinel->down;
    }

    while (sentinel && item_char_cmp(sentinel->item, c) == 0) {
        item_print(sentinel->item);
        printf("\n");
        sentinel = sentinel->next;
    }
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
    return skiplist ? 0 >= skiplist->length : 0;
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

// static LinkedStackNode *
// linked_stack_node_new(Node *node, LinkedStackNode *next) {
//     LinkedStackNode *lsn = (LinkedStackNode
//     *)malloc(sizeof(LinkedStackNode)); if (lsn) *lsn = (LinkedStackNode){.n =
//     node, .next = next}; return lsn;
// }

_Bool skiplist_includes(const SkipList *skiplist, const Item *item) {
    Node *no = skiplist_node_search(skiplist, item);
    if (NULL != no && 0 == item_cmp(no->item, item)) return 1;
    return 0;
}

Node *skiplist_node_search(const SkipList *skiplist, const Item *item) {
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
    return found_it ? sentinel : NULL;
}

static inline Node *mainlane_search(Node *sentinel, const Item *item) {
    // WARNING: We assume `NULL != sentinel` and `NULL != item`
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
    while (sentinel && sentinel->down) { // while sentinel.level > 0
        sentinel = fastlane_search(sentinel, item)->down;
    }
    return sentinel;
}

Node **skiplist_search_n_trace(SkipList *skiplist, Item *item) {
    // if (NULL == skiplist || NULL == item || NULL == skiplist->top) return
    // NULL;

    size_t n = skiplist->height;
    Node **trace = (Node **)malloc(n * sizeof(Node **));
    if (NULL == trace) return NULL;

    // Fast lane search
    Node *sentinel = skiplist->top;
    for (size_t i = 0; sentinel; i++, sentinel = sentinel->down) {
        sentinel = fastlane_search(sentinel, item);
        trace[i] = sentinel;
    }

    return trace;
}

// static status_t skiplist_push_front(SkipList *skiplist, Item *item) {
//     // New top
//     Node *new_node = node_new((Node){.item = item, .next = skiplist->top});
//     if (NULL == new_node) return NUL_ERR; // Error handling
//     Node *sentinel = skiplist->top->down;
//     skiplist->top = new_node;

//     // Creating the new nodes for each level
//     for (; sentinel; sentinel = sentinel->down) {
//         new_node->down = node_new((Node){.item = item, .next =
//         skiplist->top});

//         // Error recorvery
//         if (NULL == new_node->down) {
//             Node *sentinel = skiplist->top;
//             skiplist->top = skiplist->top->next;
//             while (sentinel) {
//                 Node *temp = sentinel->down;
//                 node_shallow_del(sentinel);
//                 sentinel = temp;
//             }
//         }
//     }

//     return SUCCESS;
// }