#include "kernel.h"
#include "kernel_funcs.h"

void kmain(void)
{
    int hold;
    int i;

    VGA_background();
    // infinite loop for debugging purposes
    hold = 1;
    while(hold){;}

    VGA_clear();
    VGA_display_str("        Hello World\n");
    VGA_display_str("        line2\r\n");
    for(i=0; i < 30; i++)
    {
        VGA_display_str("no sir\r\n");
        VGA_display_str("yes sir\r\n");
    }
    VGA_clear_row(1);
    while(1)
    {
        asm("hlt");
    }
}