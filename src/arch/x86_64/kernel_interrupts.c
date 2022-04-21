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
    // disable and remap pic
    PIC_disable;
    PIC_remap(PIC_MREMAP, PIC_SREMAP);
    // idt is currently intialized to zero
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

/*
arguments:
	offset1 - vector offset for master PIC
		vectors on the master become offset1..offset1+7
	offset2 - same for slave PIC: offset2..offset2+7
*/
void PIC_remap(int offset1, int offset2)
{
	unsigned char a1, a2;
 
	a1 = inb(PIC1_DATA);                        // save masks
	a2 = inb(PIC2_DATA);
 
	outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
	io_wait();
	outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
	io_wait();
	outb(PIC1_DATA, offset1);                 // ICW2: Master PIC vector offset
	io_wait();
	outb(PIC2_DATA, offset2);                 // ICW2: Slave PIC vector offset
	io_wait();
	outb(PIC1_DATA, 4);                       // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	io_wait();
	outb(PIC2_DATA, 2);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
	io_wait();
 
	outb(PIC1_DATA, ICW4_8086);
	io_wait();
	outb(PIC2_DATA, ICW4_8086);
	io_wait();
 
	outb(PIC1_DATA, a1);   // restore saved masks.
	outb(PIC2_DATA, a2);
}

// PIC end of interupt call
void PIC_sendEOI(unsigned char irq)
{
	if(irq >= 8)
		outb(PIC2_COMMAND,PIC_EOI);
 
	outb(PIC1_COMMAND,PIC_EOI);
}

void PIC_disable(void)
{
    asm("mov al, 0xff\
    out 0xa1, al\
    out 0x21, al");
    return;
}

void IRQ_set_mask(unsigned char IRQline)
{
    uint16_t port;
    uint8_t value;
 
    if(IRQline < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        IRQline -= 8;
    }
    value = inb(port) | (1 << IRQline);
    outb(port, value);        
}
 
void IRQ_clear_mask(unsigned char IRQline)
{
    uint16_t port;
    uint8_t value;
 
    if(IRQline < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        IRQline -= 8;
    }
    value = inb(port) & ~(1 << IRQline);
    outb(port, value);        
}

/* Helper func */
static uint16_t __pic_get_irq_reg(int ocw3)
{
    /* OCW3 to PIC CMD to get the register values.  PIC2 is chained, and
     * represents IRQs 8-15.  PIC1 is IRQs 0-7, with 2 being the chain */
    outb(PIC1_CMD, ocw3);
    outb(PIC2_CMD, ocw3);
    return (inb(PIC2_CMD) << 8) | inb(PIC1_CMD);
}
 
/* Returns the combined value of the cascaded PICs irq request register */
uint16_t pic_get_irr(void)
{
    return __pic_get_irq_reg(PIC_READ_IRR);
}
 
/* Returns the combined value of the cascaded PICs in-service register */
uint16_t pic_get_isr(void)
{
    return __pic_get_irq_reg(PIC_READ_ISR);
}

void gen_isr_handler(int irq_num, int error_code)
{
    // debug
    asm ( "hlt" );
    return;
}