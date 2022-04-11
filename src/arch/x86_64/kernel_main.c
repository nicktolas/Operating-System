#include "kernel.h"
#include "kernel_funcs.h"

void kmain(void)
{
    int hold;
    long long int big_num= 420694206942069420;
    unsigned long long int neg_big_num= -420694206942069420;

    VGA_background();
    // infinite loop for debugging purposes
    hold = 1;
    while(hold){;}

    VGA_clear();

    printk("Printk Testing\r\n");
    printk("Short 420\r\n\tshort: %d\r\n\tunsigned short: %u\r\n\thex: %x\r\n", 420, -420, 420);
    printk("Int 42069\r\n\tint: %d\r\n\tunsigned int: %u\r\n\thex: %x\r\n", 42069, -42069, 42069);
    printk("Long 42069x3\r\n\tlong int: %ld\r\n\tunsigned long int: %lu\r\n\tlong hex: %lx\r\n", 420694206942069, -420694206942069, 420694206942069);
    printk("Long Long 42069x4\r\n\tll int: %qd\r\n\tunsigned ll int: %qu\r\n\tll hex: %qx\r\n", big_num, neg_big_num, big_num);
    printk("String Testing\r\n\tUnsigned char: %c\r\n\tpointer %p\r\n\tstring:%s\r\n", -1, &hold, "Yeet");

    

    while(1)
    {
        asm("hlt");
    }
}