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

    keyboard_init();

    interrupts_init();

    // test_printk();
    test_keyboard();
    
    while(1)
    {
        asm("hlt");
    }
}