#ifndef KERNEL_MEMORY
#define KERNEL_MEMORY
#include "kernel.h"
#include "kernel_data_structs.h"
#define PAGE_LIMIT_SIZE 10000
#define PAGE_INIT_SIZE 10
#define PAGE_SIZE 4096

struct Max_Page_Info
{
    uint64_t addr;
    struct Memory_Map_Node* memory_chunk;
}__attribute((packed));

struct Physical_Page_Frame
{
    struct Node super;
    uint8_t type; // type 0 is free, type 1 is used
    uint8_t dirty;
    uint16_t reserved;
}__attribute__((packed));

// Memory Manipulation
void * memset(void *dst, int c, size_t n);
void *memcpy(void *restrict dest, const void *restrict src, size_t n);

uint8_t inb(uint16_t port);
void outb(uint16_t port, uint8_t val);

// paging
void * MMU_pf_alloc(void);
void MMU_pf_free(void *pf);
void init_physical_paging(void);
void create_availible_pages(void);
int is_valid_page(uint64_t low_limit);
void display_page_content(void* pf);
void display_pages(struct Linked_List* Page_list);
struct Physical_Page_Frame* find_page(struct Linked_List* ll, void*req_addr);
void write_page(void* pf, char* string, uint64_t length);
void print_u8_no_prefix(uint8_t num);

void init_page_frame(void* pf);

//debug functions
void debug_display_lists(void);
void display_page_frame(void* pf);

#endif