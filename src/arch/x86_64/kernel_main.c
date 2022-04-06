#include "kernel.h"
#include "kernel_funcs.h"

void kmain(void)
{
    int hold;

    // infinite loop for debugging purposes
    hold = 1;
    while(hold){;}

    VGA_clear();
    VGA_background();
    // VGA_display_str("YEET");

    while(1)
    {
        asm("hlt");
    }
}