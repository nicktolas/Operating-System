#include "kernel.h"
#include "kernel_memory.h"
#include "doors_string.h"
#include "kernel_multiboot.h"
#include "kernel_data_structs.h"

extern struct Linked_List Memory_Map_List;
extern struct Linked_List Exclusions_List;
static struct Linked_List Avail_Pages;
static struct Max_Page_Info max_page;

// Sets n bytes of memory to c starting at location defined by dst
void * memset(void *dst, int c, size_t n)
{
    int i;
    char* curr_pos = (char*)dst;
    for (i=0;i<n;i++)
    {
        curr_pos[i] = c;
        curr_pos++;
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
                if(row >= 1)
                {
                    return;
                }
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
    printk("\r\n");
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