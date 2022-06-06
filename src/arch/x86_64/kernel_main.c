#include "kernel.h"
#include "kernel_vga.h"
#include "doors_string.h"
#include "kernel_testing.h"
#include "doors_keyboard.h"
#include "kernel_interrupts.h"
#include "kernel_gdt.h"
#include "kernel_serial.h"
#include "kernel_multiboot.h"
#include "kernel_memory.h"

void kmain(void)
{
    VGA_background();
    // infinite loop for debugging purposes
    int hold = 1;
    while(hold){;}
    // VGA_clear();
    gdt_assign_segments();
    init_multiboot();
    init_physical_paging();
    init_page_tables();
    interrupts_init();
    init_kernel_dynamic_structs();
    SER_init();
    keyboard_init();
    enable_int_irq();
    asm("STI"); // enable interrupts

    test_heap();
    while(1)
    {
        asm("hlt");
    }
}