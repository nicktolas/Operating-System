#ifndef KERNEL_DATA_STRUCTS
#define KERNEL_DATA_STRUCTS
#include "kernel.h"

/* Each node will have tracking information which is used by the structure functions. 
   The void * should point to a valid structure */
struct Node
{
    struct Node* next;
    struct Node* prev;
    int data;
};

struct Linked_List
{
    struct Node* head;
    struct Node* tail;
    int length;
};

void init_node(struct Node*);
void free_node(struct Node*);

void init_linked_list(struct Linked_List* ll, struct Node* node);
void free_ll(struct Linked_List*);
struct Node* ll_get_node(struct Linked_List* ll , int req_data);
struct Node* ll_pop_node(struct Linked_List*ll, struct Node* req_node);
void ll_add_node(struct Linked_List* ll, struct Node* req_node);
void ll_del_node(struct Linked_List* ll, struct Node* req_node);
#endif