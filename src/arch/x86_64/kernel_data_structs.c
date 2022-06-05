#include "kernel_data_structs.h"
#include "kernel_memory.h"

/* ====================================================================

                            Linked List

   ==================================================================== */

/* Initializes a Node for a list. Does not allocate the node*/
void init_node(struct Node* node)
{
    node->next = NULL;
    node->prev = NULL;
    node->data = 0;
    return;
}

/* Will free the node from the memmory allocator. Currently just set to 0.*/
void free_node(struct Node* node)
{
    init_node(node);
    return;
}

/* Initializes the linked list structure itself*/
void init_linked_list(struct Linked_List* ll, struct Node* node)
{
    ll->length = 0;
    ll->head = node;
    ll->tail = node;
    return;
}

/* Frees the Linked List. Just resets the structor until allocator is done. */
void free_ll(struct Linked_List* ll)
{
    init_linked_list(ll, NULL);
    return;
}

/* Returns the requested Node from the Linked List. Matches based on requested Node data. */
struct Node* ll_get_node(struct Linked_List* ll , int req_data)
{
    struct Node* curr_node = ll->head;
    while(curr_node != NULL)
    {
        if(curr_node->data == req_data) // data match
        {
            return curr_node;
        }
        curr_node = curr_node->next;
    }
    return curr_node;
}

/* pop the requested node from the list. Will remove pointers on req_node to next and previous. */
struct Node* ll_pop_node(struct Linked_List* ll, struct Node* req_node)
{
    struct Node* prev_node = req_node->prev;
    struct Node* next_node = req_node->next;

    if(ll->length <= 0) // invalid linked list
    {
        return NULL;
    }
    else if((ll->length == 1) && (ll->head == req_node)) // node is head and the tail of our list
    {
        ll->head = NULL;
        ll->tail = NULL;
    }
    else if(ll->head == req_node) // linked list head is our node
    {
        ll->head = next_node;
    }
    else if(ll->tail == req_node) // linked list tail is our node
    {
        ll->tail = prev_node;
    }
    else // regular case | prev node - req node - next node
    {
        prev_node->next = next_node;
        next_node->prev = prev_node;
    }
    req_node->next = NULL;
    req_node->prev = NULL;
    ll->length--;
    return req_node;
}

/* Adds a node to the end of the list */
void ll_add_node(struct Linked_List* ll, struct Node* req_node)
{
    if(ll->length == 0) // list has no nodes
    {
        ll->head = req_node;
        ll->tail = req_node;
        req_node->next = NULL;
        req_node->prev = NULL;
    }
    else // list has nodes | ll->tail - req_node - NULL
    {
        req_node->prev = ll->tail;
        ll->tail->next = req_node;
        req_node->next = NULL;
        ll->tail = req_node;
    }
    ll->length++;
    return;
}

/* Pops the node from the list and frees it. */
void ll_del_node(struct Linked_List* ll, struct Node* req_node)
{
    ll_pop_node(ll, req_node);
    free_node(req_node);
    return;
}

/* ====================================================================

                            Binary Tree

   ==================================================================== */

void init_tree_node(struct Tree_Node* curr)
{
    curr->left = NULL;
    curr->right = NULL;
    curr->parent = NULL;
    curr->data = 0;
    return;
}

void init_btree(struct Binary_Tree* bt)
{
    bt->root = NULL;
    bt->length = 0;
    return;
}


uint64_t my_pow(uint64_t base, uint64_t exp)
{
    uint64_t result = 1;
    uint64_t i;
    for(i=0; i < exp; i++)
    {
        result = result * base;
    }
    return result;
}