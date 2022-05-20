#ifndef KERNEL_MULTIBOOT
#define KERNEL_MULTIBOOT
#include "kernel.h"
#include "kernel_data_structs.h"
#define MM_NODE_LIST_SIZE 100
#define EX_NODE_LIST_SIZE 100

struct Multiboot_Fixed_Header
{
    uint32_t tag_size;
    uint32_t reserved;
}__attribute__((packed));

struct Multiboot_Variable_Header
{
    uint32_t tag_type;
    uint32_t tag_size;
}__attribute__((packed));

//tag type 4
struct Multiboot_Memory_Header
{
    struct Multiboot_Variable_Header super;
    uint32_t low_memory_size;
    uint32_t high_memory_size;
}__attribute__((packed));

// tag type 5
struct Multiboot_BIOS_Boot_Device_Header
{
    struct Multiboot_Variable_Header super;
    uint32_t boot_device;
    uint32_t boot_partition;
    uint32_t boot_subpartition;
}__attribute__((packed));

// tag type 1
struct Multiboot_Boot_Command_Line_Header
{
    struct Multiboot_Variable_Header super;
    char* command_string;
}__attribute__((packed));

// tag type 2
struct Multiboot_Boot_Loader_Name_Header
{
    struct Multiboot_Variable_Header super;
    char* bootloader_string;
}__attribute__((packed));

// Entries of type 1 are free to use, else ignore the entry
struct Multiboot_Memory_Map_Entry
{
    uint64_t start_addr;
    uint64_t length;
    uint32_t type;
    uint32_t reserved;
}__attribute__((packed));

// tag type 6
struct Multiboot_Memory_Map_Header
{
    struct Multiboot_Variable_Header super;
    uint32_t entry_size;
    uint32_t entry_version;
    struct Multiboot_Memory_Map_Entry entry_head;
}__attribute__((packed));

struct Multiboot_ELF_Section_Entry
{
    uint32_t section_name_index;
    uint32_t section_type;
    uint64_t flags;
    uint64_t memory_segment_addr;
    uint64_t disk_segment_offset;
    uint64_t segment_size;
    uint32_t table_index;
    uint32_t extra_info;
    uint64_t address_alignment;
    uint64_t IFF_section;
}__attribute__((packed));

// tag type 9
struct Multiboot_ELF_Symbols_Header
{
    struct Multiboot_Variable_Header super;
    uint32_t num_entries;
    uint32_t entry_size;
    uint32_t string_table_index;
    struct Multiboot_ELF_Section_Entry entry_head;
}__attribute__((packed));

struct Memory_Map_Node
{
    struct Node super;
    uint64_t start_addr;
    uint64_t length;
}__attribute__((packed));

void init_multiboot(void);
void iterate_variable_headers(uint32_t header_len);
void parse_memory_map(uint32_t entry_size, uint32_t num_entries, struct Multiboot_Memory_Map_Entry* entries);
void parse_elf_header(int num_entries, struct Multiboot_ELF_Section_Entry* start_entry);
void display_memory_map(struct Linked_List* ll);
#endif