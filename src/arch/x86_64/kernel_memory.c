#include "kernel.h"
#include "kernel_memory.h"
#include "doors_string.h"
#include "kernel_multiboot.h"
#include "kernel_data_structs.h"

extern struct Linked_List Memory_Map_List;
extern struct Linked_List Exclusions_List;
static struct Linked_List Avail_Pages;
static struct Page_Node Page_Node_Allocation[PAGE_LIMIT_SIZE] = {0};

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
    // for each memory block
    struct Memory_Map_Node* curr_mapped_node = NULL;
    struct Page_Node* curr_page = NULL;
    int byte_offset = 0;
    curr_mapped_node = (struct Memory_Map_Node*) Memory_Map_List.head;
    while(curr_mapped_node != NULL) // iterate through availible memory sections
    {
        while(byte_offset + PAGE_SIZE < curr_mapped_node->length) // while memory still availible to chunk out
        {
            if(is_valid_page(curr_mapped_node->start_addr + byte_offset) == 0)
            {
                curr_page = &Page_Node_Allocation[Avail_Pages.length];
                curr_page->start_addr = curr_mapped_node->start_addr + byte_offset;
                curr_page->length = PAGE_SIZE;
                curr_page->dirty = 0;
                ll_add_node(&Avail_Pages, &curr_page->super);
            }
            if(Avail_Pages.length > PAGE_INIT_SIZE) // only allocates 100 pages to save on some space
            {
                return;
            }
            byte_offset += PAGE_SIZE;
        }
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

void * MMU_pf_alloc(void)
{
    return;
}

void MMU_pf_free(void *pf)
{
    return;
}

/* Displays physical pages that are availible for the system*/
void display_pages()
{
    struct Page_Node* curr_node = (struct Page_Node*) Avail_Pages.head;
    printk(" -- Availible Pages -- \r\n");
    while(curr_node != NULL)
    {
        printk("\tNode - Base Addr: %lx | Length: %ld | Dirty: %d\r\n", curr_node->start_addr, curr_node->length, curr_node->dirty);
        curr_node = (struct Page_Node*) curr_node->super.next;
    }
    return;
}