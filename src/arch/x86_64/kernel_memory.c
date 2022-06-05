#include "kernel.h"
#include "kernel_memory.h"
#include "doors_string.h"
#include "kernel_multiboot.h"
#include "kernel_data_structs.h"

extern struct Linked_List Memory_Map_List;
extern struct Linked_List Exclusions_List;
static struct Linked_List Avail_Pages;
static struct Max_Page_Info max_page;
static struct Page_Table_Entry* PT4;
uint64_t old_cr3;
uint64_t new_cr3;
uint64_t high_mem_addr;
void* last_alloced_page;

struct Kernel_Stacks
{
    void* base_addr;
    int num_stacks;
};

struct Kernel_Heap
{
    void* base_addr;
    struct Heap_Frame* base;
    uint64_t length;
};

static struct Kernel_Heap k_heap;

// static struct Kernel_Stacks k_stacks;

// Sets n bytes of memory to c starting at location defined by dst
void * memset(void *dst, int c, size_t n)
{
    // int i;
    unsigned char* curr_pos = (unsigned char*)dst;
    while (n > 0)
    {
        *curr_pos = c;
        curr_pos++;
        n--;
    }
    return dst; 
}

// // Copys n bytes of memory from dst to src. 
void *memcpy(void *restrict dest, const void *restrict src, size_t n)
{
    int i; 
    for(i=0; i < n; i++)
    {
        ((char*)dest)[i] = ((char*)src)[i];
    }
    return dest;
}

uint8_t inb(uint16_t port)
{
    uint8_t ret;
    asm volatile ( "inb %1, %0"
    : "=a"(ret)
    : "Nd"(port) );
    return ret;
}

void outb(uint16_t port, uint8_t val)
{
    asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
    return;
}

/* -------------------------------------------------------------

                        Physical Pages

   ------------------------------------------------------------- */

/* Initializes PAGE_INIT_SIZE Pages into a Linked List of size PAGE_LIMIT_SIZE*/
void init_physical_paging()
{
    max_page.addr = 0;
    max_page.memory_chunk = (struct Memory_Map_Node*) Memory_Map_List.head;
    create_availible_pages();
    return;
}

/* generates additional pages as requested*/
void create_availible_pages()
{
    // for each memory block
    struct Memory_Map_Node* curr_mapped_node = NULL;
    uint64_t byte_offset = 0;
    uint64_t target_length = Avail_Pages.length + PAGE_INIT_SIZE;
    if(max_page.addr != 0)
    {
        byte_offset = max_page.addr - (uint64_t)max_page.memory_chunk->start_addr;
        byte_offset += 4096; // first page after the one we allocated
    }
    curr_mapped_node = max_page.memory_chunk;
    while(curr_mapped_node != NULL) // iterate through availible memory sections
    {
        
        while(curr_mapped_node->start_addr + byte_offset + PAGE_SIZE < curr_mapped_node->length) // while memory still availible to chunk out
        {
            if(is_valid_page(curr_mapped_node->start_addr + byte_offset) == 0)
            {
                
                init_page_frame((void*)(curr_mapped_node->start_addr + byte_offset));
                max_page.addr = curr_mapped_node->start_addr + byte_offset;
                max_page.memory_chunk = curr_mapped_node;
            }
            if((PAGE_INIT_ALL != 1) && (Avail_Pages.length >= target_length)) // only allocates 100 pages to save on some space
            {
                return;
            }
            byte_offset += PAGE_SIZE;
        }
        byte_offset = 0;
        
        curr_mapped_node = (struct Memory_Map_Node*) curr_mapped_node->super.next;
    }
    return;
}

/* Gets the largest memory address on the system*/
uint64_t get_high_mem_addr()
{
    uint64_t max_val = 0;
    struct Memory_Map_Node* curr;
    curr = (struct Memory_Map_Node*) Memory_Map_List.tail;
    max_val = curr->start_addr + curr->length;
    return max_val;
}

/* Iterates through the exclusions list to determine if the requested base_address can be allocated.
   Returns 0 if page is able to be used and 1 otherwise. */
int is_valid_page(uint64_t low_limit)
{
    struct Memory_Map_Node* curr_ex_node = (struct Memory_Map_Node*) Exclusions_List.head;
    uint64_t upper_limit = low_limit + PAGE_SIZE;
    while(curr_ex_node != NULL) // could sort list to do a speedup on this if we want
    {
        if(!((low_limit > (curr_ex_node->start_addr + curr_ex_node->length)) || (upper_limit < curr_ex_node->start_addr))) // if low limit above range or high limit below range 
        {
            return 1;
        } 
        curr_ex_node = (struct Memory_Map_Node*) curr_ex_node->super.next;   
    }
    return 0;
}

/* Allocates a Page of memory for use */
void * MMU_pf_alloc(void)
{
    struct Physical_Page_Frame* curr_page = NULL;
    if(Avail_Pages.length <= 0) // only 1 page left to allocate, get some more
    {
        create_availible_pages(); // use base addr to know where we fetched last time (list in order)
        if(Avail_Pages.length <= 0)
        {
            return NULL;
        }
    }
    curr_page = (struct Physical_Page_Frame*) ll_pop_node(&Avail_Pages, Avail_Pages.head); // pop node
    // ll_add_node(&Used_Pages, &curr_page->super); // add to used
    curr_page->type = 1; // now used
    last_alloced_page = (void*)curr_page;
    return (void *) curr_page;
}

/* Frees the requested pageframe */
void MMU_pf_free(void *pf)
{
    init_page_frame(pf);
    return;
}

/* Find the respective node in the Page Linked List */
struct Physical_Page_Frame* find_page(struct Linked_List* ll, void*req_addr)
{
    struct Physical_Page_Frame* curr_page = NULL;
    curr_page = (struct Physical_Page_Frame*) ll->head;
    if(req_addr == (void*) ll->tail)
    {
        return curr_page;
    }
    while(curr_page != (struct Physical_Page_Frame*) ll->tail)
    {
        if((uint64_t)curr_page == (uint64_t) req_addr)
        {
            return curr_page;
        }
        curr_page = (struct Physical_Page_Frame*) curr_page->super.next;
    }
    return NULL;
}

/* Intializes the page frame structure onto page itself and adds it to the allocated list*/
void init_page_frame(void* pf)
{
    struct Physical_Page_Frame* ppf = (struct Physical_Page_Frame*) pf;
    memset(pf, 0, sizeof(struct Physical_Page_Frame)); // set the frame to 0 prior to use
    ppf->dirty = 0; 
    ppf->type = 0; // free
    ppf->reserved = 0; // allignment
    ll_add_node(&Avail_Pages, &ppf->super);
    return;
}

/* Writes to the requested page */
void write_page(void* pf, char* string, uint64_t length)
{
    int max_len = length;
    // used to verify we write only to existing pages
    if(max_len >= PAGE_SIZE - sizeof(struct Physical_Page_Frame) - 1)
    {
        max_len = PAGE_SIZE;
        return;
    }
    memcpy((void*)((uint64_t)pf), string, max_len);
    return;
}

/* Dumps the contents of a page_frame*/
void display_page_content(void* pf)
{
    uint64_t offset = 0;
    uint8_t val = 0;
    int row = 0;
    printk("\r\n-- Page at %p --\r\n", pf);
    for(offset = 0; offset<PAGE_SIZE; offset = offset + 1)
    {
        if(offset % 64 == 0)
        {
            if(row < 10)
            {
                printk("\r\n Row 0%d: ", row);
            }
            else
            {
                printk("\r\n Row %d: ", row);
            }
            row++;
        }
        if(offset % 4 == 0)
        {
            printk(" ");
        }
        memcpy(&val, (void*)((uint64_t)pf + offset), 1);
        print_u8_no_prefix(val);
    }
    // printk("\r\n");
    return;
}

/* Dumps the contents of a page_frame*/
void display_page_frame(void* pf)
{
    uint64_t offset = 0;
    uint8_t val = 0;
    struct Physical_Page_Frame* ppf = (struct Physical_Page_Frame*) pf;
    printk("\r\n-- Page Frame Information %p --\r\n", pf);
    printk("Node Struct - Next: %p - Prev: %p - Data: %x | Type: %x - Dirty: %x - Reserved: %x\r\n-> ", 
    ppf->super.next, ppf->super.prev, ppf->super.data, ppf->type, ppf->dirty, ppf->reserved);
    for(offset = 0; offset<sizeof(struct Physical_Page_Frame); offset = offset + 1)
    {
        if((offset % 4 == 0 ) && (offset != 0))
        {
            printk(" ");
        }
        memcpy(&val, (void*)((uint64_t)pf + offset), 1);
        print_u8_no_prefix(val);
    }
    printk("\r\n");
    return;
}

/* Always prints the u32 value in a fixed 8 len size*/
void print_u8_no_prefix(uint8_t num)
{
    int total_len = 0;
    char string_of_int[UTOA_BUF_SIZE] = {0};
    char adjusted_string[UTOA_BUF_SIZE] = {0};
    uitoa(num, string_of_int, 16);
    if((total_len = strlen(string_of_int)) == 2) // if string is not 8 in size
    {
        adjusted_string[1] = string_of_int[0];
        adjusted_string[0] = '0';
        adjusted_string[3] = '\0';
        display_string_wrapper(adjusted_string);
    }
    else
    {
        display_string_wrapper(string_of_int);
    }
    return;
}

/* Displays physical pages that are availible for the system*/
void display_pages(struct Linked_List* Page_list)
{
    struct Physical_Page_Frame* curr_node = (struct Physical_Page_Frame*) Page_list->head;
    while(curr_node != NULL)
    {
        printk("\tNode - Base Addr: %p | Dirty: %d\r\n", curr_node, curr_node->dirty);
        // display_page_content((void* ) curr_node->start_addr);
        curr_node = (struct Physical_Page_Frame*) curr_node->super.next;
    }
    return;
}

void debug_display_lists(void)
{
    printk("Availible Pages\r\n");
    display_pages(&Avail_Pages);
    // printk("Used Pages\r\n");
    // display_pages(&Used_Pages);
    return;
}

/* -------------------------------------------------------------

                        Virtual Memory & Page Tables

   ------------------------------------------------------------- */

/* Initalizes the virtual page tables needed for paging.
   Sets up the identity map for our page table as the only init entry  */
void init_page_tables()
{
    setup_P4_entry(0);
    old_cr3 = 0;
    new_cr3 = 0;
    // read CR3
    asm volatile ("mov %%cr3, %0"
    :"=r"(old_cr3)
    :
    :);
    // load the table into CR3 register
    asm volatile ("movq %0, %%cr3;"
    :
    : "r"(PT4)
    );
    asm volatile ("mov %%cr3, %0"
    :"=r"(new_cr3)
    :
    :);
    return;
}

/* Sets up the identity map from PT4, only 2 entries needed to house all of memory space for ID map */
void setup_P4_entry(uint64_t vaddr)
{
    struct Page_Table_Entry* entry;
    int i;
    void* PT3;
    PT4 = MMU_pf_alloc();
    high_mem_addr = get_high_mem_addr();
    memset((void*) PT4, 0, PAGE_SIZE);
    for(i=0; i<512; i++) // only 2 entries needed for ID map
    {
        entry = (struct Page_Table_Entry*) PT4 + i;
        // setup p3 entry
        PT3 = (void*)((uint64_t) setup_P3_entry(vaddr + i*PT4_ADDR_OFFSET) >> 12);
        // setup the p3 addr -- returned from p3 entry function
        entry->pt_base_addr_l4 = ((uint64_t)PT3) & 0xF;
        entry->pt_base_addr_20_5 = (((uint64_t)PT3) >> 4) & 0xFFFF; // 16 bit value
        entry->pt_base_addr_36_21 = (((uint64_t)PT3) >> 20) & 0xFFFF; // 16 bit value
        entry->present = 1;
        entry->writable = 1;
        if((vaddr + i*PT4_ADDR_OFFSET) >= high_mem_addr)
        {
            break;
        }
    }
    return;
}

/* Sets up the identity map from PT3 */
void* setup_P3_entry(uint64_t vaddr)
{
    struct Page_Table_Entry* entry;
    struct Page_Table_Entry* PT3;
    int i;
    void* PT2;
    PT3 = MMU_pf_alloc();
    memset((void*)PT3, 0, PAGE_SIZE);
    for (i=0; i<512; i++)
    {
        entry = (struct Page_Table_Entry*) PT3 + i;
        // Instantiate all of PT2 below
        PT2 = (void*)((uint64_t)setup_P2_entry(vaddr + i*PT3_ADDR_OFFSET) >> 12);
        // setup the p2 addr -- returned from p2 entry function
        entry->pt_base_addr_l4 = ((uint64_t)PT2) & 0xF;
        entry->pt_base_addr_20_5 = (((uint64_t)PT2) >> 4) & 0xFFFF; // 16 bit value
        entry->pt_base_addr_36_21 = (((uint64_t)PT2) >> 20) & 0xFFFF; // 16 bit value
        entry->present = 1;
        entry->writable = 1;
        if((vaddr + i*PT3_ADDR_OFFSET) >= high_mem_addr)
        {
            break;
        }
    }
    return (void*) PT3;
}

/* Sets up the identity map from PT2 */
void* setup_P2_entry(uint64_t vaddr)
{
    struct Page_Table_Entry* entry;
    struct Page_Table_Entry* PT2;
    int i;
    void* PT1;
    PT2 = MMU_pf_alloc();
    memset((void*)PT2, 0, PAGE_SIZE);
    // printk("PT2 is %p\r\n", (void*)PT2);
    for (i=0; i<512; i++)
    {
        entry = (struct Page_Table_Entry*) PT2 + i;
        // Instantiate all of PT1 below
        PT1 = (void*)((uint64_t)setup_P1_entry(vaddr + i*PT2_ADDR_OFFSET) >> 12);
        // setup the p2 addr -- returned from p2 entry function
        entry->pt_base_addr_l4 = ((uint64_t)PT1) & 0xF;
        entry->pt_base_addr_20_5 = (((uint64_t)PT1) >> 4) & 0xFFFF; // 16 bit value
        entry->pt_base_addr_36_21 = (((uint64_t)PT1) >> 20) & 0xFFFF; // 16 bit value
        entry->present = 1;
        entry->writable = 1;
        if((vaddr + i*PT2_ADDR_OFFSET) >= high_mem_addr)
        {
            break;
        }
    }
    return PT2;
}

/* Sets up the identity map from PT1 */
void* setup_P1_entry(uint64_t vaddr)
{
    struct Page_Table_Entry* PT1;
    struct Page_Table_Entry* entry;
    int i;
    PT1 = MMU_pf_alloc();
    // printk("\tAvail Pages Head: %p | PT1 is %p\r\n", (void*)Avail_Pages.head, (void*)PT1);
    memset((void*)PT1, 0, PAGE_SIZE);
    // Identiy map the bottom pages
    for (i=0; i<512; i++)
    {
        entry = PT1 + i;
        entry->present = 1;
        entry->writable = 1;
        entry->pt_base_addr_l4 = ((vaddr + i*PT1_ADDR_OFFSET) >> 12) & 0xF;
        entry->pt_base_addr_20_5 = (((vaddr + i*PT1_ADDR_OFFSET) >> 12) >> 4) & 0xFFFF; // 16 bit value
        entry->pt_base_addr_36_21 = (((vaddr + i*PT1_ADDR_OFFSET) >> 12) >> 20) & 0xFFFF; // 16 bit value
        if((vaddr + i*PT1_ADDR_OFFSET) >= high_mem_addr)
        {
            break;
        }
    }
    if(vaddr == 0) // ensure page fault on NULL address
    {
        PT1->present = 0;
    }
    return PT1;
}

/* Walks the page table to resolve the virtual address (vaddr)
   Free = zero: when you are looking to allocate the address if not present
   Free = nonzero: when you are looking to free the allocated address       */
void walk_vaddr_page_four(uint64_t vaddr, int free)
{
    struct Page_Table_Entry* entry;
    void* PT3 = 0;
    int index;
    index = (vaddr >> 39) & 0x1FF; // 9 bit mask
    entry = (struct Page_Table_Entry*) PT4 + index;
    // printk("PT4\r\n");
    // debug_filled_entries((struct Page_Table_Entry*) PT4);
    if((entry->present == 0) && (free == 0)) // does not have any pages below it, setup p4 entry and p3 entry
    {
        PT3 = walk_vaddr_page_three(vaddr, NULL, free);
        entry->pt_base_addr_l4 = ((uint64_t)PT3) & 0xF;
        entry->pt_base_addr_20_5 = (((uint64_t)PT3) >> 4) & 0xFFFF; // 16 bit value
        entry->pt_base_addr_36_21 = (((uint64_t)PT3) >> 20) & 0xFFFF; // 16 bit value
        entry->present = 1;
        entry->writable = 1;
    }
    else
    {
        PT3 = (void*) (((uint64_t)(entry->pt_base_addr_36_21 << 20) | (entry->pt_base_addr_20_5 << 4) | (entry->pt_base_addr_l4)) << 12);
        walk_vaddr_page_three(vaddr, PT3, free);
    }
    return;
}

/* Walks the page table to resolve the virtual address (vaddr)
   Free = zero: when you are looking to allocate the address if not present
   Free = nonzero: when you are looking to free the allocated address       */
void* walk_vaddr_page_three(uint64_t vaddr, void* old_PT3, int free)
{
    struct Page_Table_Entry* entry;
    void* PT2 = 0;
    void* PT3;
    int index;
    index = (vaddr >> 30) & 0x1FF; // 9 bit mask
    if(old_PT3 == NULL) // page table doesnt exist, setup the table
    {
        PT3 = MMU_pf_alloc();
        memset(PT3, 0, PAGE_SIZE);
        // printk("Alloc'd new PT3\r\n");
    }
    else
    {
        PT3 = old_PT3;
    }
    // printk("PT3\r\n");
    // debug_filled_entries((struct Page_Table_Entry*) PT3);
    entry = (struct Page_Table_Entry*) PT3 + index;
    if((entry->present == 0) && (free == 0)) // does not have present entry
    {
        PT2 = walk_vaddr_page_two(vaddr, NULL, free); // below page does not have entry
        entry->pt_base_addr_l4 = ((uint64_t)PT2) & 0xF;
        entry->pt_base_addr_20_5 = (((uint64_t)PT2) >> 4) & 0xFFFF; // 16 bit value
        entry->pt_base_addr_36_21 = (((uint64_t)PT2) >> 20) & 0xFFFF; // 16 bit value
        entry->present = 1;
        entry->writable = 1;
    }
    else 
    {
        PT2 = (void*) (((uint64_t)(entry->pt_base_addr_36_21 << 20) | (entry->pt_base_addr_20_5 << 4) | (entry->pt_base_addr_l4)) << 12);
        walk_vaddr_page_two(vaddr, PT2, free);
    }
    return (void*)((uint64_t)PT3 >> 12);
}

/* Walks the page table to resolve the virtual address (vaddr)
   Free = zero: when you are looking to allocate the address if not present
   Free = nonzero: when you are looking to free the allocated address       */
void* walk_vaddr_page_two(uint64_t vaddr, void* old_PT2, int free)
{
    struct Page_Table_Entry* entry;
    void* PT1 = 0;
    void* PT2; 
    int index;
    index = (vaddr >> 21) & 0x1FF; // 9 bit mask
    if(old_PT2 == NULL) // page table doesnt exist, setup the table
    {
        PT2 = MMU_pf_alloc();
        memset(PT2, 0, PAGE_SIZE);
        // printk("Alloc'd new PT2 %p\r\n", PT2);
    }
    else
    {
        PT2 = old_PT2;
    }
    // printk("PT2\r\n");
    // debug_filled_entries((struct Page_Table_Entry*) PT2);
    entry = (struct Page_Table_Entry*) PT2 + index;
    if((entry->present == 0) && (free == 0)) // does not have present entry
    {
        PT1 = walk_vaddr_page_one(vaddr, NULL, free); // below page does not have entry
        entry->pt_base_addr_l4 = ((uint64_t)PT1) & 0xF;
        entry->pt_base_addr_20_5 = (((uint64_t)PT1) >> 4) & 0xFFFF; // 16 bit value
        entry->pt_base_addr_36_21 = (((uint64_t)PT1) >> 20) & 0xFFFF; // 16 bit value
        entry->present = 1;
        entry->writable = 1;
    }
    else
    {
        PT1 = (void*) (((uint64_t)(entry->pt_base_addr_36_21 << 20) | (entry->pt_base_addr_20_5 << 4) | (entry->pt_base_addr_l4)) << 12);
        walk_vaddr_page_one(vaddr, PT1, free);
    }
    return (void*)((uint64_t)PT2 >> 12);
}

/* Walks the page table to resolve the virtual address (vaddr)
   Free = zero: when you are looking to allocate the address if not present
   Free = nonzero: when you are looking to free the allocated address       */
void* walk_vaddr_page_one(uint64_t vaddr, void* old_PT1, int free)
{
    struct Page_Table_Entry* entry;
    void* page = 0;
    void* PT1;
    int index;
    index = (vaddr >> 12) & 0x1FF; // 9 bit mask
    if(old_PT1 == NULL) // page table doesnt exist, setup the table
    {
        PT1 = MMU_pf_alloc();
        memset(PT1, 0, PAGE_SIZE);
        // printk("Alloc'd new PT1\r\n");
    }
    else
    {
        PT1 = old_PT1;
    }
    // printk("PT1\r\n");
    // debug_filled_entries((struct Page_Table_Entry*) PT1);
    entry = (struct Page_Table_Entry*) PT1 + index;
    if((entry->present == 0) && (free == 0)) // does not have present entry
    {
        page = MMU_pf_alloc(); 
        memset(page, 0, PAGE_SIZE);
        entry->pt_base_addr_l4 = ((uint64_t)page >> 12) & 0xF;
        entry->pt_base_addr_20_5 = (((uint64_t)page >> 12) >> 4) & 0xFFFF; // 16 bit value
        entry->pt_base_addr_36_21 = (((uint64_t)page >> 12) >> 20) & 0xFFFF; // 16 bit value
        entry->present = 1;
        entry->writable = 1;
        // entry->write_cache = 1;
        // printk("Allocated page %p for virtual address %lx\r\n", page, vaddr);
    }
    else if((entry->present == 1) && (free == 1)) // free an allocated item
    {
        page = 0;
        page = PT1 = (void*) (((uint64_t)(entry->pt_base_addr_36_21 << 20) | (entry->pt_base_addr_20_5 << 4) | (entry->pt_base_addr_l4)) << 12);
        // printk("Freeing Page %p for virtual address %lx\r\n", page, vaddr);
        MMU_pf_free(page);
        entry->present = 0;
    }
    else // previously allocated address
    {
        if(entry->present == 1)
        {
            printk("Tried Allocating an existing entry: %lx with index %d\r\n", vaddr, index);
        }
        else if(free == 1) // should never be hit?
        {
            printk("Tried to free an non present entry: %lx\r\n", vaddr);
        }
        return NULL; // this shouldnt happen
    }
    return (void*)((uint64_t)PT1 >> 12);
}

/* Page Fault ISR */
void page_fault_isr(int error_code)
{
    uint64_t cr2;
    uint64_t cr3;
    asm volatile ("mov %%cr3, %0"
    :"=r"(cr3)
    :
    :);
    asm volatile ("mov %%cr2, %0"
    :"=r"(cr2)
    :
    :);
    // printk("Page Table 4 Address: %lx\r\nAddress that caused the fault: %lx\r\n", cr3, cr2);
    alloc_vaddr((void*)cr2); // allocate the page
    return;
}

void alloc_vaddr(void* vaddr)
{
    walk_vaddr_page_four((uint64_t)vaddr, 0);
    return;
}

void free_vaddr(void* vaddr)
{
    walk_vaddr_page_four((uint64_t)vaddr, 1);
    return;
}

// debug functions

void dump_page_addresses()
{
    printk("Old CR3 address: %lx\r\nNew CR3 address: %lx\r\n", old_cr3, new_cr3);
    return;
}

void debug_allocator(void)
{
    int* vaddr = (int*)0xDEADBEEF;
    dump_page_addresses();
    printk("Testing the following Address %p\r\n", vaddr);
    // printk("PT4 Offset: %ld PT3 Offset: %ld PT2 Offset: %ld PT1 Offset: %ld\r\n",
    // (vaddr >> 39) & 0x1FF, (vaddr >> 30) & 0x1FF, (vaddr >> 21) & 0x1FF, (vaddr >> 12) & 0x1FF);
    
    // printk("Double Allocating %lx\r\n", vaddr);
    // walk_vaddr_page_four(vaddr, 0);
    // printk("Freeing %lx\r\n", vaddr);
    // walk_vaddr_page_four(vaddr, 1);
    // printk("Double Freeing %lx\r\n", vaddr);
    // walk_vaddr_page_four(vaddr, 1);
    // printk("Allocating %lx\r\n", vaddr);
    // walk_vaddr_page_four(vaddr, 0);
    printk("\r\nassigning to int....\r\n\r\n");
    *vaddr = 420;
    // display_page_content(last_alloced_page);
    printk("\t---The value we have is %d\r\n", *vaddr);
    *vaddr = 420420;
    // display_page_content(last_alloced_page);
    printk("\t---The value we have is %d\r\n", *vaddr);
    printk("\r\nFree the address....\r\n\r\n");
    free_vaddr((void*)vaddr);
    printk("\r\nassigning a another time....\r\n\r\n");
    *vaddr = 420420420;
    printk("\t---The value we have is %d\r\n", *vaddr);
    printk("\r\nassigning a another time, no fault please!\r\n\r\n");
    *vaddr = 420;
    // display_page_content(last_alloced_page);
    printk("\t---The value we have is %d\r\n", *vaddr);
    return;
}

void debug_filled_entries(struct Page_Table_Entry* PT)
{
    struct Page_Table_Entry* entry;
    int i;
    uint64_t next;
    printk("------- Page Table Walk --------\r\n");
    for (i=0; i < 512; i++)
    {
        entry = PT + i;
        next = 0;
        next = ((uint64_t)((entry->pt_base_addr_36_21 << 20) | (entry->pt_base_addr_20_5 << 4) | (entry->pt_base_addr_l4))) << 12;
        if(entry-> present == 1)
        {
            printk("\tTable Entry %d: PRESENT | NEXT: %lx\r\n", i, next);
        }
    }
}




/* -------------------------------------------------------------

                        Kernel Dynamic Memory

   ------------------------------------------------------------- */

void init_kernel_dynamic_structs()
{
    init_kernel_heap();
    return;
}

void init_kernel_stacks()
{
    return;
}


/* Initalizes the heap frame that will rest on a page*/
static void init_heap_frame(struct Heap_Frame* hf, int ord)
{
    hf->allocated = 0;
    hf->order = ord;
    return;
}

void init_kernel_heap()
{
    k_heap.base = (void*)MMU_KHEAP_MAP; //virtual address used as base of heap (grows down)
    k_heap.length = 1; // starts with nothing in the list
    // printk("Setting the Initial Node for our binary tree at addr %p ...\r\n", k_heap.base_addr);
    init_heap_frame(k_heap.base, MAX_BUDDY_ORDER);
    return;
}

static void debug_heap_frame(struct Heap_Frame* hf)
{
    printk("Heap Frame %p: Allocated %d Order %d\r\n",(void*)hf, hf->allocated, hf->order);
    return;
}



/* Helper Function to create new memory blocks for a parent */
static int create_buddy(struct Heap_Frame* parent)
{
    int new_order = parent->order - 1;
    long int address_offset;
    struct Heap_Frame* rchild;
    if(new_order < 0) // cant go below 0
    {
        return -1;
    }
    rchild = (struct Heap_Frame*) (((uint64_t)parent) + ((my_pow(2, new_order))* PAGE_SIZE));
    address_offset = (my_pow(2, new_order))* PAGE_SIZE;
    init_heap_frame( (struct Heap_Frame*) ((uint64_t)parent), new_order);                                   // l child frame
    init_heap_frame( rchild, parent->order); // r child frame
    k_heap.length++;
    printk("\tAddress Offset should be %lx\r\n \r\n", address_offset);
    printk("\tSplit L child %p into order %d\r\n", (void*)parent, parent->order);
    printk("\tSplit R child %p into order %d\r\n", (void*)rchild, rchild->order);
    return parent->order;
}

/* Iterates over the memory space until it finds a fit for the allocation. 
   Returns the smallest order that fits the requested size allocation.
   This will split the tree into smaller order blocks until order 0 reached. 
   If no block can be found, returns NULL. */
static void* buddy_find_home(struct Heap_Frame* parent, int desired_order)
{
    void* found_addr = NULL;
    printk("Finding Home: Parent %p of Order %d, with desired order as %d\r\n", (void*)parent, parent->order, desired_order);
    // parent can fit and is unallocated
    if((parent->order == desired_order) && (parent->allocated == 0))
    {
        parent->allocated = 1;
        found_addr = (void*)(parent++); // returns the address at end of pointer
        printk("Found a fit of order %d at %p with final addr as %p\r\n", parent->order, (void*)parent, found_addr);
        return found_addr;
    }
    // current block can be split into smaller blocks
    if ((parent->order > desired_order) && (parent->allocated == 0))
    {
        if(create_buddy(parent) < 0)
        {
            printk("Unable to split parent into smaller block\r\n");
            while(1)
                asm("hlt;");
        }
        // block is split into smaller one, call this function again
        return buddy_find_home(parent, desired_order);
    }
    printk("Walking Heap: Order %d with offset %lx\r\n",parent->order, ((my_pow(2, parent->order))* PAGE_SIZE) );
    return buddy_find_home( (struct Heap_Frame*) (((uint64_t)parent) + ((my_pow(2, parent->order))* PAGE_SIZE)) , desired_order);
}

/* Determines the pool size needed for the object itself*/
static int find_pool(size_t true_size)
{
    int order = 0;
    while(order < MAX_BUDDY_ORDER+1)
    {
        if(true_size <= ((my_pow(2, order))*PAGE_SIZE))
        {
            return order;
        }
        order++;
    }
    printk("Failed to determine order, size of object: %lu\r\n", true_size);
    return -1;
}

/* Returns a virtually continugous block of memory that is of size 2^n * 1k where n is the small order
   in which req_size can fit into. This a buddy allocator. Largest memory allocation is 2^4*1024k
   Returns NULL if req_size bytes cannot be contiguously allocated.                                  */
void* kmalloc(size_t req_size)
{
    void* return_addr = NULL;
    size_t true_size = req_size + sizeof(struct Heap_Frame);
    return_addr = buddy_find_home(k_heap.base, find_pool(true_size));
    return return_addr; // should always be NULL
}

/*  Prints the heap in a tree like structure*/
void print_heap()
{
    struct Heap_Frame* curr = k_heap.base;
    int i = 0;
    int j = 0;
    printk("\r\n|-------------| Heap Layout |-----------|\r\n\r\n");
    for(i=0; i < k_heap.length; i++)
    {
        for(j=MAX_BUDDY_ORDER; j > curr->order; j--)
        {
            printk("<--");
        }
        debug_heap_frame(curr);
        curr = (struct Heap_Frame*) (((uint64_t)curr) + ((my_pow(2, curr->order))* PAGE_SIZE));
    }
    printk("\r\n|---------------------------------------|\r\n");
    return;
}
