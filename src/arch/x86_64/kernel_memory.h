#ifndef KERNEL_MEMORY
#define KERNEL_MEMORY
#include "kernel.h"
#include "kernel_data_structs.h"
#define PAGE_LIMIT_SIZE 1000
#define PAGE_INIT_SIZE 100
#define PAGE_SIZE 4096
struct Page_Node
{
    struct Node super;
    uint64_t start_addr;
    uint64_t length;
    bool dirty;
}__attribute__((packed));

// Memory Manipulation
void * memset(void *dst, int c, size_t n);
void *memcpy(void *restrict dest, const void *restrict src, size_t n);

uint8_t inb(uint16_t port);
void outb(uint16_t port, uint8_t val);

// paging
void init_physical_paging(void);
int is_valid_page(uint64_t low_limit);
void display_pages(void);
#endif