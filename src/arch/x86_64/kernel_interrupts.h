#ifndef KERNEL_INTERRUPTS
#define KERNEL_INTERRUPTS
#define IDT_LENGTH 256

struct Call_Gate_Descriptor
{   
    uint16_t target_offset_bot;
    uint16_t target_selector;
    uint16_t stack_target:3;
    uint16_t reserved_two:4;
    uint16_t int_trap_gate:1;
    uint16_t one:3;
    uint16_t zero:1;
    uint16_t protection_level:3;
    uint16_t present:1;
    uint16_t target_offset_mid;
    uint32_t target_offset_top;
    uint32_t reserved_one;  
}__attribute__((packed));

void interrupts_init(void);
void idt_init(void);
void load_idt();
void occupy_idt(void);

#endif