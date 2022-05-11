#ifndef KERNEL_GDT
#include "kernel.h"
#define KERNEL_GDT
#define GDT_TSS_OFFSET 0x28
#define GDT_NUM_SEGMENTS 8
#define GDT_NULL_SEGMENT 0
#define GDT_KCODE_SEGMENT 1
#define GDT_KDATA_SEGMENT 2
#define GDT_UCODE_SEGMENT 3
#define GDT_UDATA_SEGMENT 4
#define GDT_TSS_SEGMENT 5 // takes 2 positions


struct x86_64_GDT
{
    uint64_t segments[GDT_NUM_SEGMENTS];
    int next_free;
}__attribute__((packed));


struct Segment_Descriptor
{
    uint16_t limit_l16;
    uint16_t base_l16;
    uint8_t base_M8;
    uint8_t access_byte;
    uint8_t limit_h4:4;
    uint8_t flags:4;
    uint8_t base_H8;
}__attribute__((packed));


struct System_Segment_Descriptor
{
    uint16_t limit_l16;
    uint16_t base_l16;
    uint8_t base_ml8;
    uint8_t access_byte;
    uint8_t limit_h4:4;
    uint8_t flags:4;
    uint8_t base_mh8;
    uint32_t base_h32;
    uint32_t reserved;
}__attribute__((packed));

// Structure used for the TSS
struct TSS_Descriptor
{
    uint16_t segement_limit_bot;
    uint16_t base_addr_L16;
    uint8_t base_addr_MidL8;
    uint8_t type:4;
    uint8_t zero:1;
    uint8_t dpl:2;
    uint8_t present:1;
    uint8_t segement_limit_top:4;
    uint8_t avl:1;
    uint8_t reserved_one:2;
    uint8_t granularity;
    uint8_t base_addr_MidH8;
    uint32_t base_addr_H32;
    uint8_t reserved_two;
    uint8_t zero_two:5;
    uint8_t reserved_three:3;
    uint16_t reserved_four;
}__attribute__((packed));

struct Task_State_Segment
{
    uint32_t reserved_one;
    uint32_t rsp0_low;
    uint32_t rsp0_high;
    uint32_t rsp1_low;
    uint32_t rsp1_high;
    uint32_t rsp2_low;
    uint32_t rsp2_high;
    uint32_t reserved_two;
    uint32_t reserved_three;
    uint32_t ist1_low;
    uint32_t ist1_high;
    uint32_t ist2_low;
    uint32_t ist2_high;
    uint32_t ist3_low;
    uint32_t ist3_high;
    uint32_t ist4_low;
    uint32_t ist4_high;
    uint32_t ist5_low;
    uint32_t ist5_high;
    uint32_t ist6_low;
    uint32_t ist6_high;
    uint32_t ist7_low;
    uint32_t ist7_high;
    uint32_t reserved_four;
    uint32_t reserved_five;
    uint16_t reserved_six;
    uint16_t io_map_base_addr;
}__attribute__((packed));

void gdt_assign_segments(void);
void gdt_setup_ksegments(void);
void gdt_setup_TSS(void);
void load_GDT_TSS(void);
void reload_cs_reg();

#endif
