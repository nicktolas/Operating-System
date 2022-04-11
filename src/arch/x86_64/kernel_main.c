#include "kernel.h"
#include "kernel_funcs.h"
#include "doors_string.h"
#include "kernel_testing.h"
#include "doors_keyboard.h"

void kmain(void)
{
    int hold;

    VGA_background();
    // infinite loop for debugging purposes
    hold = 1;
    while(hold){;}

    VGA_clear();
    // test_printk();
    test_keyboard();
    
    while(1)
    {
        asm("hlt");
    }
}