#ifndef KERNEL_MEMORY
#define KERNEL_MEMORY
#include "kernel.h"
#include "kernel_data_structs.h"
#define PAGE_LIMIT_SIZE 10000
#define PAGE_INIT_SIZE 10
#define PAGE_INIT_ALL 1
#define PAGE_SIZE 4096
#define MMU_ID_MAP 0x0
#define MMU_KSTACK_MAP 0x10000000000
#define MMU_KGROWTH_MAP 0x20000000000
#define MMU_KHEAP_MAP 0xF0000000000
#define MMU_USER_MAP 0x100000000000
#define MMU_KSTACK_SIZE 2000
#define MMU_USTACK_SIZE 4000
#define MMU_PAGE_TABLE_SIZE 512
#define PT1_ADDR_OFFSET PAGE_SIZE
#define PT2_ADDR_OFFSET 2097152
#define PT3_ADDR_OFFSET 1073741824
#define PT4_ADDR_OFFSET 549755813888
#define MB_TO_PAGE 1000

struct Heap_Frame
{
    struct Node curr;
    size_t size;
}__attribute__((packed));

struct Max_Page_Info
{
    uint64_t addr;
    struct Memory_Map_Node* memory_chunk;
}__attribute__((packed));

struct Physical_Page_Frame
{
    struct Node super;
    uint8_t type; // type 0 is free, type 1 is used
    uint8_t dirty;
    uint16_t reserved;
}__attribute__((packed));

struct Page_Table_Entry
{
    uint8_t present:1;
    uint8_t writable:1;
    uint8_t perm:1;
    uint8_t write_cache:1;
    uint8_t cache_disable:1;
    uint8_t access:1;
    uint8_t dirty:1;
    uint8_t page_size:1; // end 8 byte bountry
    uint8_t global:1;
    uint8_t avail:3;
    uint8_t pt_base_addr_l4: 4; // end 8
    uint16_t pt_base_addr_20_5; // 16
    uint32_t pt_base_addr_36_21:20; 
    uint32_t avail_top:11; 
    uint32_t NX:1; // 32 byte
}__attribute__((packed));

// Memory Manipulation
void * memset(void *dst, int c, size_t n);
void *memcpy(void *restrict dest, const void *restrict src, size_t n);

uint8_t inb(uint16_t port);
void outb(uint16_t port, uint8_t val);

// physical paging
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
uint64_t get_high_mem_addr();
// virtual paging
void init_page_tables(void);
void setup_P4_entry(uint64_t vaddr);
void* setup_P3_entry(uint64_t vaddr);
void* setup_P2_entry(uint64_t vaddr);
void* setup_P1_entry(uint64_t vaddr);
void walk_vaddr_page_four(uint64_t vaddr, int free);
void* walk_vaddr_page_three(uint64_t vaddr, void* PT3, int free);
void* walk_vaddr_page_two(uint64_t vaddr, void* PT2, int free);
void* walk_vaddr_page_one(uint64_t vaddr, void* PT1, int free);

void page_fault_isr(int error_code);
void alloc_vaddr(void* vaddr);
void free_vaddr(void* vaddr);

// kernel dynamic structures

void init_kernel_dynamic_structs(void);
void init_kernel_stacks();
void init_kernel_heap();
void* kmalloc(size_t req_size);

//debug functions
void debug_display_lists(void);
void display_page_frame(void* pf);

void dump_page_addresses(void);
void debug_allocator(void);
void debug_filled_entries(struct Page_Table_Entry* PT);
#endif