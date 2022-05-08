#ifndef KERNEL_INTERRUPTS
#define KERNEL_INTERRUPTS
#define IDT_LENGTH 256
#define PIC1 0x20		/* IO base address for master PIC */
#define PIC2 0xA0		/* IO base address for slave PIC */
#define PIC1_COMMAND PIC1
#define PIC1_DATA (PIC1+1)
#define PIC2_COMMAND PIC2
#define PIC2_DATA (PIC2+1)
#define PIC_EOI 0x20
#define ICW1_ICW4 0x01		/* ICW4 (not) needed */
#define ICW1_SINGLE	0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4 0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL 0x08		/* Level triggered (edge) mode */
#define ICW1_INIT 0x10		/* Initialization - required! */
#define ICW4_8086 0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO 0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE 0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */
#define ICW4_SFNM 0x10		/* Special fully nested (not) */
#define PIC1_CMD PIC1_COMMAND
#define PIC2_CMD PIC2_COMMAND
#define PIC_READ_IRR 0x0a    /* OCW3 irq ready next CMD read */
#define PIC_READ_ISR 0x0b    /* OCW3 irq service next CMD read */
#define PIC_MREMAP 0x20 /* remaps to 0x20 - 0x27 */
#define PIC_SREMAP 0x70 /* remaps to 0x70 - 0x77 */
#define MASK_BITS_L32 0xFFFFFFFF
#define MASK_BITS_L16 0xFFFF
#define CGD_TRAP 1
#define CGD_INT 0

// used for the IDT
struct Call_Gate_Descriptor
{   
    uint16_t target_offset_bot;
    uint16_t target_selector;
    uint8_t stack_target:3;
    uint8_t reserved_two:5;
    uint8_t int_trap_gate:1;
    uint8_t one:3;
    uint8_t zero:1;
    uint8_t protection_level:2;
    uint8_t present:1;
    uint16_t target_offset_mid;
    uint32_t target_offset_top;
    uint32_t reserved_one;  
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

// true represents interrupts on, false represents they are off
bool interrupt_status;

void interrupts_init(void);
void idt_init(void);
void load_idt();
void occupy_idt(void);
void PIC_sendEOI(unsigned char irq);
void PIC_remap(int offset1, int offset2);
void PIC_disable(void);
void PIC_enable(void);
void IRQ_clear_mask(unsigned char IRQline);
void IRQ_set_mask(unsigned char IRQline);
uint16_t pic_get_isr(void);
uint16_t pic_get_irr(void);
uint16_t __pic_get_irq_reg(int ocw3);
void gen_isr_handler(int irq_num, int error_code);
void PIC_init(void);
void keyboard_int_handler(void);
void enable_int_irq(void);
void setup_TSS(void);

static inline void interrupt_off(void)
{
    asm("sti");
}

static inline void interrupt_on(void)
{
    asm("cli");
}

#endif