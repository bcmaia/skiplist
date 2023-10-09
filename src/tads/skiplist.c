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
static void node_shallow_del(Node **node);

/**
 * @brief frees the memory used to store a node and its contetns.
 * @warning if item is used elsewhere after a node containing a reference
 *  to it if deleted using this function, a segfault by dangling ptr access may
 *  ocur.
 *  leak.
 *
 * @param node
 */
static void node_deep_del(Node **node);

/**
 * @brief Creates a linked list stack node.
 *
 * @param node ptr which will be stored in this linked list node.
 * @param next ptr to the next linked list node.
 * @return LinkedStackNode* ptr to the new LinkedStackNode, WITH OWNERSHIP, or
 * NULL on error.
 */
static LinkedStackNode *
linked_stack_node_new(Node *node, LinkedStackNode *next);

static inline Node *mainlane_search(Node *sentinel, const Item *item);

static inline Node *fastlane_search(Node *sentinel, const Item *item);

static inline Node *express_search(Node *sentinel, const Item *item);

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
            node_shallow_del(&car);
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
        node_deep_del(&titanic);
        titanic = temp_titanic; // go to bellow
    }

    free(*skiplist);
    *skiplist = NULL;
}

status_t skiplist_insert(SkipList *skiplist, Item *item) {
    // Err handling when null poiter
    // WARNING: other erros such as invalid ptr will not be caught.
    if (NULL == skiplist || NULL == item) return NUL_ERR;

    LinkedStackNode *trace = NULL;
    Node *sentinel = skiplist->top;

    // Trivial case: empty list
    if (NULL == sentinel) {
        skiplist->top = node_new((Node){.item = item});
        if (NULL == skiplist->top) return ALLOC_ERR;

        skiplist->height = 1;
        skiplist->length = 1;

        return SUCCESS;
    }

    // Trivial case: item is the same to start
    int diff = item_cmp(sentinel->item, item);
    if (0 == diff) return REPEATED_ENTRY_ERR;

    // Trivial case: insert at start
    if (diff > 0) {
        Node *up = node_new((Node){.item = item, .next = sentinel});
        if (NULL == up) return ALLOC_ERR;
        skiplist->top = up;
        sentinel = sentinel->down;

        while (NULL != sentinel) {
            Node *new_node = node_new((Node){.item = item, .next = sentinel});
            // TODO: Error handling here
            // if (NULL == new_node)
            up->down = new_node;
            up = new_node;
        }
        skiplist->length++;
        return SUCCESS;
    }

    // Search in fast lanes
    while (NULL != sentinel->down) { // while sentinel.level > 0
        // predecessor = NULL;
        while (sentinel->next && item_cmp(sentinel->next->item, item) < 0) {
            sentinel = sentinel->next;
        }
        trace = linked_stack_node_new(sentinel, trace);
        sentinel = sentinel->down;
    }

    // Search in main lane
    Node *predecessor = sentinel;
    while (sentinel && item_cmp(sentinel->item, item) < 0) {
        predecessor = sentinel;
        sentinel = sentinel->next;
    }

    // Error: repeated entry
    if (sentinel && 0 == item_cmp(sentinel->item, item)) { // repeated
        while (NULL != trace) {
            LinkedStackNode *temp = trace->next;
            free(trace);
            trace = temp;
        }

        return REPEATED_ENTRY_ERR;
    }

    Node *new_node = node_new((Node){.item = item, .next = sentinel});
    predecessor->next = new_node;

    Node *down = new_node;
    while ((NULL != trace) && (rand() & 1)) {
        // clang-format off
        new_node = node_new(
            (Node){
                .item = item, 
                .next = trace->n->next, 
                .down = down
            }
        );
        // clang-format on

        // TODO: error handling here (new_node might be null)
        trace->n->next = new_node;

        LinkedStackNode *temp = trace->next;

        free(trace);

        trace = temp;
        down = new_node;
    }

    // Raise level
    // clang-format off
    #ifdef SKIPLIST_MAX_HEIGHT
        _Bool raise_level =
            ((NULL == trace) && (rand() & 1) &&
            (skiplist_height(skiplist) < SKIPLIST_MAX_HEIGHT));
    #else
        _Bool raise_level = ((NULL == trace) && (rand() & 1));
    #endif
    // clang-format on

    if (raise_level) {
        new_node = node_new((Node){.item = item, .next = NULL, .down = down});
        skiplist->top = node_new((Node
        ){.item = skiplist->top->item, .next = new_node, .down = skiplist->top}
        );
        skiplist->height++;
    }

    // Free remaining trace
    while (NULL != trace) {
        LinkedStackNode *temp = trace->next;
        free(trace);
        trace = temp;
    }

    skiplist->length++;

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
    while (sentinel->down) { // while sentinel.level > 0
        while (sentinel->next && item_cmp(sentinel->next->item, item) < 0) {
            sentinel = sentinel->next;
        }
        sentinel = sentinel->down;
    }

    // Search in main lane
    while (sentinel && item_cmp(sentinel->item, item) < 0) {
        sentinel = sentinel->next;
    }

    _Bool found_it = sentinel && 0 == item_cmp(sentinel->item, item);
    return found_it ? sentinel->item : NULL;
}

// Temporary disable the "-Wunused-parameter" warning.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

_Bool skiplist_is_full(const SkipList *skiplist) {
    // clang-format off
    #ifdef SKIPLIST_MAX_LENGTH
        #define aaaa
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

static void node_shallow_del(Node **node) {
    if (NULL == node) return;
    free(*node);
    *node = NULL;
}

static void node_deep_del(Node **node) {
    if (NULL == node) return;
    item_del(&(*node)->item);
    free(*node);
    *node = NULL;
}

static LinkedStackNode *
linked_stack_node_new(Node *node, LinkedStackNode *next) {
    LinkedStackNode *lsn = (LinkedStackNode *)malloc(sizeof(LinkedStackNode));
    if (lsn) *lsn = (LinkedStackNode){.n = node, .next = next};
    return lsn;
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
    while (sentinel->down) { // while sentinel.level > 0
        while (sentinel->next && item_cmp(sentinel->next->item, item) < 0) {
            sentinel = sentinel->next;
        }
        sentinel = sentinel->down;
    }

    // Search in main lane
    while (sentinel && item_cmp(sentinel->item, item) < 0) {
        sentinel = sentinel->next;
    }

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
    while (sentinel->next && item_cmp(sentinel->next->item, item) < 0) {
        sentinel = sentinel->next;
    }
    return sentinel;
}

static inline Node *express_search(Node *sentinel, const Item *item) {
    // WARNING: We assume `NULL != sentinel` and `NULL != item`
    while (sentinel->down) { // while sentinel.level > 0
        sentinel = fastlane_search(sentinel, item)->down;
    }
    return sentinel;
}