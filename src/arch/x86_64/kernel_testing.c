#include "kernel.h"
#include "doors_string.h"
#include "kernel_memory.h"
#include "kernel_vga.h"
#include "doors_keyboard.h"

void test_printk(void)
{
    long long int big_num = 420694206942069420;
    unsigned long long int neg_big_num = -420694206942069420;
    printk("Printk Testing\r\n");
    printk("Short 420\r\n\tshort: %hd\r\n\tunsigned short: %hu\r\n\thex: %hx\r\n", -420, 420, 420);
    printk("Int 42069\r\n\tint: %d\r\n\tunsigned int: %u\r\n\thex: %x\r\n", -42069, 42069, 42069);
    printk("Long 42069x3\r\n\tlong int: %ld\r\n\tunsigned long int: %lu\r\n\tlong hex: %lx\r\n", -420694206942069, 420694206942069, 420694206942069);
    printk("Long Long 42069x4\r\n\tll int: %qd\r\n\tunsigned ll int: %qu\r\n\tll hex: %qx\r\n", neg_big_num, big_num, big_num);
    printk("String Testing\r\n\tUnsigned char: %c\r\n\tpointer %p\r\n\tstring:%s\r\n", -1, &big_num, "Yeet");
}

void test_keyboard(void)
{
    keyboard_loop();
    return;
}