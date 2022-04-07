#include "kernel.h"
#include "kernel_funcs.h"

void kmain(void)
{
    int hold;

    VGA_background();
    // infinite loop for debugging purposes
    hold = 1;
    while(hold){;}

    VGA_clear();
    VGA_display_str("        Hello World\n");
    VGA_display_str("        line2\r");
    VGA_display_str("no sir\n");
    VGA_clear_row(1);

    while(1)
    {
        asm("hlt");
    }
}