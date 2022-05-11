#include "kernel_memory.h"
#include "kernel_gdt.h"
#include <stdint-gcc.h>
#include "kernel_interrupts.h"

extern void reloadSegments();

struct x86_64_GDT GDT;
struct Task_State_Segment TSS;
char GP_Int_Stack[4096] = {0};
char PF_Int_Stack[4096] = {0};
char DF_Int_Stack[4096] = {0};

/* Loads the GDT and task register*/
void load_GDT_TSS()
{
    static struct
    {
        uint16_t length;
        void* base;
    }__attribute__((packed)) gdt_ptr;
    int tss_offset = GDT_TSS_OFFSET;
    gdt_ptr.length = (sizeof(uint64_t)*GDT_NUM_SEGMENTS)-1;
    gdt_ptr.base = &(GDT.segments);
    asm ("lgdt %0" :: "m"(gdt_ptr));
    asm("ltr %0":: "m"(tss_offset));
    reload_cs_reg();
    return;
}

/* Loads the new GDT into the cs register*/
void reload_cs_reg()
{
    reloadSegments();
    return;
}

/* The intialization function of our new gdt*/
void gdt_assign_segments()
{
    memset(&GDT.segments, 0, sizeof(uint64_t)*GDT_NUM_SEGMENTS);
    // NULL is already setup (should just be 0)
    gdt_setup_ksegments();
    //TODO: Setup UserSpace Segements
    gdt_setup_TSS();
    return;
}


/*Sets up both the kernel code and data segment.
  The Code segment is 4 Bytes and Data is 8 Bytes*/
void gdt_setup_ksegments()
{
    struct Segment_Descriptor* segment;

    // Kernel Code Segment
    segment = (struct Segment_Descriptor*) &(GDT.segments[GDT_KCODE_SEGMENT]);
    memset(segment, 0, 4);
    segment->access_byte = 0x4E; // 1001110
    segment->flags = 0x2; // 0010

    // Kernel Data Segment
    segment = (struct Segment_Descriptor*) &(GDT.segments[GDT_KDATA_SEGMENT]);
    memset(segment, 0, 4);
    segment->access_byte = 0x4A; // 1001010
    segment->flags = 0x2; // 0010
    return;
}

/* Setup the TSS descriptor values to map to a valid entry for GDT
    Some of the entries are not needed to be filled*/
void gdt_setup_TSS()
{
    struct System_Segment_Descriptor* segment = (struct System_Segment_Descriptor*) &(GDT.segments[GDT_TSS_SEGMENT]);
    segment->limit_l16 = sizeof(struct Task_State_Segment);
    segment->base_l16 = (uint64_t)&TSS & MASK_BITS_L16;
    segment->base_ml8 = ((uint64_t)&TSS & MASK_BITS_L24) >> 16; // gets bits 16-23
    segment->access_byte = 0x89; // 10001001
    segment->base_mh8 = ((uint64_t)&TSS & MASK_BITS_L32) >> 24; // gets bits 24-31
    segment->base_h32 = ((uint64_t)&TSS) >> 32; // gets bits 32-63

    // setup the Task State Segmment
    /* Configures the segement itself with valid stacks for each fault */
    TSS.ist1_low = (uint64_t)&GP_Int_Stack & MASK_BITS_L32;
    TSS.ist1_high = (uint64_t)&GP_Int_Stack >> 32;
    TSS.ist2_low = (uint64_t)&DF_Int_Stack & MASK_BITS_L32;
    TSS.ist2_high = (uint64_t)&DF_Int_Stack >> 32;
    TSS.ist3_low = (uint64_t)&PF_Int_Stack & MASK_BITS_L32;
    TSS.ist3_high = (uint64_t)&PF_Int_Stack >> 32;
    return;
}
