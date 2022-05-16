#include "kernel.h"
#include "kernel_vga.h"
#include "doors_string.h"
#include "kernel_testing.h"
#include "doors_keyboard.h"
#include "kernel_interrupts.h"
#include "kernel_gdt.h"
#include "kernel_serial.h"
#include "kernel_multiboot.h"

void kmain(void)
{
    multiboot_header_location();
    VGA_background();
    // infinite loop for debugging purposes
    int hold = 1;
    while(hold){;}
    VGA_clear();
    gdt_assign_segments();
    interrupts_init();
    SER_init();
    keyboard_init();
    enable_int_irq();
    asm("STI"); // enable interrupts
    
    init_multiboot();

    while(1)
    {
        asm("hlt");
    }
}