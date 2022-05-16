#include "kernel_memory.h"
#include "kernel_multiboot.h"
#include "kernel_data_structs.h"

struct Linked_List Memory_Map_List;
struct Node node_section[MM_NODE_LIST_SIZE] = {0};
static void* multiboot_addr;

/* Only to be used at intialization in kmain. Gets the address of multiboot header from reg ebx. */
void multiboot_header_location(void)
{
    asm("mov %0, %%rbx": "=r"(multiboot_addr):);
    return;
}

/* Consumes the multiboot headers from the address grabbed at*/
void init_multiboot()
{ 
    int byte_offset = 0;
    struct Multiboot_Fixed_Header* mfh = (struct Multiboot_Fixed_Header*) multiboot_addr;
    byte_offset += 8;
    // converts address to int, adds byte offset

    iterate_variable_headers(mfh->tag_size);
    return;
}

/* Helper function to iterate through all entries*/
void iterate_variable_headers(uint32_t header_length)
{
    int byte_offset = 8; // base var header pointer
    struct Multiboot_Variable_Header* mvh;
    while(byte_offset < header_length)
    {
        mvh = (struct Multiboot_Variable_Header*) (((uint64_t) multiboot_addr) + byte_offset);
        byte_offset += mvh->tag_size;
    }
    return;
}