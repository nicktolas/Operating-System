#include "kernel.h"
#include "kernel_interrupts.h"
#include "kernel_memory.h"

struct Call_Gate_Descriptor Int_Desc_Table_Entries[256] = {0};

void interrupts_init(void)
{
    return;
}

void idt_init(void)
{
    load_idt();
    return;
}
void occupy_idt(void)
{
    int i;
    struct Call_Gate_Descriptor temp_desc;
    // may be too short? idk tired
    for(i=0; i < IDT_LENGTH-1; i++)
    {
        temp_desc.zero = 0;
    }
    return;
}

void load_idt(void)
{
    static struct
    {
        uint16_t length;
        void* base;
    }__attribute__((packed)) idt;
    idt.length = IDT_LENGTH - 1;
    idt.base = Int_Desc_Table_Entries;
    asm ( "lidt %0" : : "m"(idt) );
    return;
}

void gen_interrupt_handler(int irq_num, int error_code)
{
    // debug
    asm ( "hlt" );
    asm( "iretq" );
}