#include "kernel_memory.h"
#include "kernel_gdt.h"
#include <stdint-gcc.h>
#include "kernel_interrupts.h"

struct x86_64_GDT GDT;
struct Task_State_Segment TSS;
char GP_Int_Stack[4096] = {0};
char PF_Int_Stack[4096] = {0};
char DF_Int_Stack[4096] = {0};

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

    // load segment into the ltr register
    // asm("ltr %0":: "m"(GDT_TSS_OFFSET));
    return;
}

// struct TSS_Descriptor* TSS_desc = (struct TSS_Descriptor*) &(GDT.segments[GDT_TSS_SEGMENT]);
// TSS_desc->segement_limit_bot = sizeof(TSS) & MASK_BITS_L16;
// TSS_desc->base_addr_L16 = (uint64_t)&TSS & MASK_BITS_L16;
// TSS_desc->base_addr_MidL8 = ((uint64_t)&TSS & MASK_BITS_L24) >> 16;
// TSS_desc->type = 9;
// TSS_desc->zero = 0;
// TSS_desc->dpl = 0;
// TSS_desc->present = 1;
// TSS_desc->base_addr_MidH8 = ((uint64_t)&TSS & MASK_BITS_L32) >> 24;
// TSS_desc->base_addr_H32 = (uint64_t)&TSS >> 32;
// TSS_desc->zero_two = 0;