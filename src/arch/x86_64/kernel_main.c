#include "kernel.h"
#include "kernel_vga.h"
#include "doors_string.h"
#include "kernel_testing.h"
#include "doors_keyboard.h"
#include "kernel_interrupts.h"

void kmain(void)
{
    int hold;
    
    VGA_background();
    // infinite loop for debugging purposes
    hold = 1;
    while(hold){;}

    VGA_clear();
    interrupts_init();
    keyboard_init();
    enable_int_irq();
    asm("STI");
    
    int *deadbeef = (void*)0xdeadbeef;
    // goodbye world
    *deadbeef = 1234;
    
    // test_printk();
    // test_keyboard();
    // asm("int $128");
    while(1)
    {
        asm("hlt");
    }
}