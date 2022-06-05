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

struct Tree_Node
{
    struct Tree_Node* left;
    struct Tree_Node* right;
    struct Tree_Node* parent;
    int data;
};

struct Binary_Tree
{
    int length;
    struct Tree_Node* root;
};

void init_node(struct Node*);
void free_node(struct Node*);

void init_linked_list(struct Linked_List* ll, struct Node* node);
void free_ll(struct Linked_List*);
struct Node* ll_get_node(struct Linked_List* ll , int req_data);
struct Node* ll_pop_node(struct Linked_List*ll, struct Node* req_node);
void ll_add_node(struct Linked_List* ll, struct Node* req_node);
void ll_del_node(struct Linked_List* ll, struct Node* req_node);

void init_tree_node(struct Tree_Node* curr);
void init_btree(struct Binary_Tree* bt);

//random

uint64_t my_pow(uint64_t base, uint64_t exp);
#endif