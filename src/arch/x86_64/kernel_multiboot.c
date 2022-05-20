#include "kernel_memory.h"
#include "kernel_multiboot.h"
#include "kernel_data_structs.h"
#include "doors_string.h"

struct Linked_List Memory_Map_List;
struct Memory_Map_Node node_section[MM_NODE_LIST_SIZE] = {0};
struct Linked_List Exclusions_List;
struct Memory_Map_Node exclusions_nodes[EX_NODE_LIST_SIZE] = {0};
extern void* multiboot_addr;


/* Consumes the multiboot headers from the address grabbed at*/
void init_multiboot()
{ 
    
    struct Multiboot_Fixed_Header* mfh = (struct Multiboot_Fixed_Header*) multiboot_addr;
    printk("Multiboot Header at %p\r\ntotal length: %d\r\n", multiboot_addr, mfh->tag_size);
    iterate_variable_headers(mfh->tag_size);
    printk("-- Memory Map List -- \r\n");
    display_memory_map(&Memory_Map_List);
    printk("-- Exclusions List -- \r\n");
    display_memory_map(&Exclusions_List);
    return;
}

/* Helper function to iterate through all entries*/
void iterate_variable_headers(uint32_t header_length)
{
    int byte_offset = 8; // base var header pointer
    struct Multiboot_Variable_Header* mvh;
    struct Multiboot_Boot_Command_Line_Header* mbcl;
    struct Multiboot_Boot_Loader_Name_Header* boot_name;
    struct Multiboot_Memory_Map_Header* mmh;
    struct Multiboot_ELF_Symbols_Header* elfs;
    while(byte_offset < header_length)
    {
        
        mvh = (struct Multiboot_Variable_Header*) (((uint64_t) multiboot_addr) + byte_offset);
        // printk("Byte Offset %d, new addr %p\r\n", byte_offset, mvh);
        switch(mvh->tag_type)
        {
            case 0: // end tag
                printk("Tag Type 0: End of tags\r\n");
                printk("\tEntry Length: %d\r\n", mvh->tag_size);
                return;
            case 1:
                mbcl = (struct Multiboot_Boot_Command_Line_Header*) mvh;
                printk("Tag Type 1: Boot Command Line\r\n");
                printk("\tEntry Length: %d\r\n", mvh->tag_size);
                printk("\tCommand line is: %s\r\n", (char*)&mbcl->command_string);
                break;
            case 2: // boot loader name
                boot_name = (struct Multiboot_Boot_Loader_Name_Header*) mvh;
                printk("Tag Type 2: Bootloader name\r\n");
                printk("\tEntry Length: %d\r\n", mvh->tag_size);
                printk("\tBootloader name is: %s\r\n", (char*)&(boot_name->bootloader_string));
                break;
            case 4: // Flags tag
                printk("Tag Type 4: Flags Tag\r\n");
                printk("\tEntry Length: %d\r\n", mvh->tag_size);
                break;
            case 5: // framebuffer tag
                printk("Tag Type 5: Framebuffer Tag of multiboot header\r\n");
                printk("\tEntry Length: %d\r\n", mvh->tag_size);
                break;
            case 6: // Memory map
                mmh = (struct Multiboot_Memory_Map_Header*) mvh;
                printk("Tag Type 6: Memory Map\r\n");
                printk("\tEntry Length: %d\r\n", mvh->tag_size);
                printk("\tVersion: %d | Entry Size: %d\r\n", mmh->entry_version, mmh->entry_size);
                printk("\tTotal Entries: %d\r\n", (mvh->tag_size-16)/mmh->entry_size);
                parse_memory_map(mmh->entry_size, mmh->super.tag_size, &mmh->entry_head);
                break;
            case 8: // framebuffer info
                printk("Tag Type 8: Framebuffer Info\r\n");
                printk("\tEntry Length: %d\r\n", mvh->tag_size);
                break;
            case 9: // EFI Entry addr
                elfs = (struct Multiboot_ELF_Symbols_Header*) mvh;
                printk("Tag Type 9: EFI Entry\r\n");
                printk("\tEntry Length: %d\r\n", mvh->tag_size);
                printk("\tNumber of Entries: %d\r\n\tEntry Size: %d\r\n\tString Table Index: %d\r\n", \
                elfs->num_entries, elfs->entry_size, elfs->string_table_index);
                parse_elf_header(elfs->num_entries, &elfs->entry_head);
                break;
            case 10: // APM table
                printk("Tag Type 10: APM Table\r\n");
                printk("\tEntry Length: %d\r\n", mvh->tag_size);
                break;
            case 14: // ACPI
                printk("Tag Type 14: ACPI old RSDP\r\n");
                printk("\tEntry Length: %d\r\n", mvh->tag_size);
                break;
            case 21: // Image load base physical addr
                printk("Tag Type 21: Base Physical addr\r\n");
                break;
            default:
                printk("Tag Type %d Tag Size %d\r\n", mvh->tag_type, mvh->tag_size);
                break;
        }
        byte_offset += mvh->tag_size;
        if ((byte_offset % 8) != 0) // not 8 byte alligned
        {
            byte_offset += 8 - (byte_offset % 8); // add remainder
        }
    }
    return;
}

/* Parses the Multiboot Memory Map Tags and adds open slots to free list*/
void parse_memory_map(uint32_t entry_size, uint32_t tag_size, struct Multiboot_Memory_Map_Entry* start_entry)
{
    int byte_count = 0;
    struct Memory_Map_Node* curr_node;
    struct Multiboot_Memory_Map_Entry* entry = start_entry;
    while(byte_count+16 < tag_size)
    {
        printk("\tEntry %p:\r\n\t\tType: %d\r\n\t\tStart Addr: %p\r\n\t\tLength: %lx\r\n", entry, entry->type, (void*)entry->start_addr, entry->length);
        if(Memory_Map_List.length >= MM_NODE_LIST_SIZE)
        {
            break;
        }
        if(entry->type == 1) // free to use in OS
        {
            // add entry to the node section (allocated area)
            curr_node = (struct Memory_Map_Node*) &node_section[Memory_Map_List.length];
            init_node(&curr_node->super);
            curr_node->start_addr = entry->start_addr;
            curr_node->length = entry->length;
            ll_add_node(&Memory_Map_List, &curr_node->super);
        }
        byte_count += sizeof(struct Multiboot_Memory_Map_Entry);
        entry = (struct Multiboot_Memory_Map_Entry*) ((uint64_t)start_entry + byte_count); // iterates to next entry
    }
    return;
}

void parse_elf_header(int num_entries, struct Multiboot_ELF_Section_Entry* start_entry)
{
    int byte_count = 0;
    int i;
    struct Memory_Map_Node* curr_node;
    struct Multiboot_ELF_Section_Entry* entry = start_entry;
    for(i=0; i < num_entries; i++)
    {
        printk("\tEntry %p:\r\n\t\tSection Index: %d | Section Type: %d\r\n\
\t\tFlags: %lx | Segment Addr: %lx\r\n\t\tSegment Offset: %ld | Segment Size: %ld\r\n\
\t\tTable Index: %d | Extra Info: %d\r\n\t\t Address Allignment: %ld | IFF Section: %lx\r\n",
        entry, entry->section_name_index, entry->section_type, entry->flags, entry->memory_segment_addr,
        entry->disk_segment_offset, entry->segment_size, entry->table_index, entry->extra_info, entry->address_alignment,
        entry->IFF_section);
        byte_count += sizeof(struct Multiboot_ELF_Section_Entry);
        curr_node = &exclusions_nodes[i];
        init_node(&curr_node->super);
        curr_node->start_addr = entry->memory_segment_addr;
        curr_node->length = entry->segment_size;
        ll_add_node(&Exclusions_List, &curr_node->super);
        entry = (struct Multiboot_ELF_Section_Entry*) ((uint64_t)start_entry + byte_count);
    }
    return;
}

void display_memory_map(struct Linked_List* ll)
{
    struct Memory_Map_Node* curr_node;
    curr_node = (struct Memory_Map_Node*) ll->head;
    while(curr_node != NULL)
    {
        printk("\tNODE:\r\n\t\tStart Addr: %p\r\n\t\tLength (Bytes): %lx\r\n", (void*) curr_node->start_addr, curr_node->length);
        curr_node = (struct Memory_Map_Node*) curr_node->super.next;
    }
    return;
}