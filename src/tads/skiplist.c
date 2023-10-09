#include "tads/skiplist.h"

//=============================================================================/
//=================|    Data Structures     |==================================/
//=============================================================================/

typedef struct _node_s {
    Item *item;
    struct _node_s *next;
    struct _node_s *down;
} Node;

typedef struct _linked_stack_node_s {
    Node *n;
    struct _linked_stack_node_s *next;
} LinkedStackNode;

struct _skiplist_s {
    Node *top;
    long long unsigned length;
    unsigned height;
};

//=============================================================================/
//=================|    Private Function Declarations     |====================/
//=============================================================================/
static Node *node_new(const Node base);
static void node_shallow_del(Node **node);
static void node_deep_del(Node **node);
// static Node* node_shallow_copy(const Node* base);
// static inline Node *node_shell_copy(const Node *base);

// static Node *node_row_search(Node *node, const Item *item);

static LinkedStackNode *linked_stack_node_new(Node *node,
                                              LinkedStackNode *next) {
    LinkedStackNode *lsn = (LinkedStackNode *)malloc(sizeof(LinkedStackNode));
    if (lsn) *lsn = (LinkedStackNode){.n = node, .next = next};
    return lsn;
}

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
        new_node = node_new(
            (Node){.item = item, .next = trace->n->next, .down = down});
        
        // TODO: error handling here (new_node might be null)
        
        trace->n->next = new_node;

        LinkedStackNode *temp = trace->next;

        free(trace);

        trace = temp;
        down = new_node;
    }

// Raise level
#ifdef SKIPLIST_MAX_HEIGHT
    unsigned char raise_level =
        ((NULL == trace) && (rand() & 1) &&
         (skiplist_height(skiplist) < SKIPLIST_MAX_HEIGHT));
#else
    unsigned char raise_level = ((NULL == trace) && (rand() & 1));
#endif

    if (raise_level) {
        new_node = node_new((Node){.item = item, .next = NULL, .down = down});
        skiplist->top = node_new((Node){.item = skiplist->top->item,
                                        .next = new_node,
                                        .down = skiplist->top});
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

void skiplist_debug_print(SkipList *skiplist) {
    if (NULL == skiplist) return;

    int lv = skiplist->height - 1;
    for (Node *digger = skiplist->top; NULL != digger; digger = digger->down) { // loop
        Node *runner = digger;
        printf("lv %d) ", lv--);
        while (NULL != runner) { // loop throw the nodes in a row
            printf("[");
            item_print(runner->item);
            printf("]; ");
            runner = runner->next;
        }
        printf("\n");
        
    }
}

Item *skiplist_search(SkipList *skiplist, Item *item) {
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

    unsigned char found_it = sentinel && 0 == item_cmp(sentinel->item, item);
    return found_it ? sentinel->item : NULL;
}

//=============================================================================/
//=================|    Private Function Implementations    |==================/
//=============================================================================/
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
static void node_shallow_del(Node **node) {
    free(*node);
    *node = NULL;
}

/**
 * @brief frees the memory used to store a node and its contetns.
 * @warning if item is used elsewhere after a node containing a reference
 *  to it if deleted using this function, a segfault by dangling ptr access may
 *  ocur.
 *  leak.
 *
 * @param node
 */
static void node_deep_del(Node **node) {
    item_del(&(*node)->item);
    free(*node);
    *node = NULL;
}

// Node* node_shallow_copy(const Node* base) {
//     return base ? node_new((Node){
//         .item = base->item,
//         .next = base->next,
//         .down = base->down
//     }) : NULL;
// }

//
static inline Node *node_shell_copy(const Node *base) {
    // WARNING: We assume (base != NULL)
    return node_new((Node){
        .item = base->item,
    });
}

/**
 * @brief
 *
 * @param node a valid pointer to a node.
 * @param item
 * @return Node*
 *
 * @note This function assumes that both node and item are valid pointers.
 *  Dangling pointers, null pointer, incorrect casted pointers may cause it to
 *  miss behave.
 */
// static Node* node_row_search (Node* node, const Item* item) {
//     // NOTE (b): err handling would be reduntand here. Lets assume it works.
//     // if (NULL == node || NULL == item) return NULL;

//     // Node* last = node;
//     // Node* n
//     // while (NULL != node && item_cmp(node->item, item) < 0) {
//     //     node = node->next;
//     // }
//     // return node;
// }