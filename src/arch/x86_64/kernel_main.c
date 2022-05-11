#include "kernel.h"
#include "kernel_vga.h"
#include "doors_string.h"
#include "kernel_testing.h"
#include "doors_keyboard.h"
#include "kernel_interrupts.h"
#include "kernel_gdt.h"

void kmain(void)
{

    
    VGA_background();
    // infinite loop for debugging purposes
    int hold = 1;
    while(hold){;}
    VGA_clear();
    gdt_assign_segments();
    interrupts_init();
    keyboard_init();
    enable_int_irq();

    
    asm("STI");
    
    // int *deadbeef = (void*)0xdeadbeef;
    // // goodbye world
    // *deadbeef = 1234;

    // asm("int $128");
    while(1)
    {
        asm("hlt");
    }
}