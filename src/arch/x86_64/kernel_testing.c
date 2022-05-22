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

void test_paging(void)
{
    void* test_pages[200] = {0};
    int i;
    printk("\r\nTesting Paging\r\n");
    printk("\r\nInitialize 10 Pages\r\n");
    for(i=0; i < 10; i++)
    {
        test_pages[i] = MMU_pf_alloc();
    }
    debug_display_lists();
    printk("Free the 5th Page: %p\r\n", test_pages[4]);
    MMU_pf_free(test_pages[4]);
    debug_display_lists();
    printk("Allocate 10 more pages - should have 1 left in the avail list\r\n");
    for(i=10; i < 20; i++)
    {
        test_pages[i] = MMU_pf_alloc();
    }
    debug_display_lists();
    // display_page_frame(test_pages[0]);
    // display_page_frame(test_pages[1]);
    // display_page_frame(test_pages[2]);

    write_page(test_pages[0], "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA", 33);
    write_page(test_pages[1], "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB", 33);
    write_page(test_pages[2], "CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC", 33);

    display_page_content(test_pages[0]);
    display_page_content(test_pages[1]);
    display_page_content(test_pages[2]);
    return;
}