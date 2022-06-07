#include "kernel.h"
#include "doors_string.h"
#include "kernel_memory.h"
#include "kernel_vga.h"
#include "doors_keyboard.h"

void* test_pages[50000] = {0};

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
    
    int i;
    int max_entries = 0;
    // char buf[50] = {0};
    // printk("\r\nTesting Paging\r\n");
    // printk("\r\nInitialize All Pages\r\n");
    for(i=0; i < 50000; i++)
    {
        if((test_pages[i] = MMU_pf_alloc()) == NULL)
        {
            max_entries = i;
            // printk("Max entries is: %d", max_entries);
            break;
        }
    }
    // debug_display_lists();
    // printk("Free the 5th Page: %p\r\n", test_pages[4]);
    // MMU_pf_free(test_pages[4]);
    // debug_display_lists();
    // printk("Allocate 10 more pages - should have 1 left in the avail list\r\n");
    // for(i=10; i < sizeof(test_pages); i++)
    // {
    //     test_pages[i] = MMU_pf_alloc();
    // }
    // debug_display_lists();
    // display_page_frame(test_pages[0]);
    // display_page_frame(test_pages[1]);
    // display_page_frame(test_pages[2]);
    // printk("\r\nWriting to all pages\r\n");
    for(i=0; i < max_entries+1; i++)
    {
        memset(test_pages[i], 0, PAGE_SIZE);
    }
    for(i=0; i < max_entries+1; i ++)
    {
        MMU_pf_free(test_pages[i]);
    }
    // for(i=0; i < 50000; i = i + 2500)
    // {
    //     display_page_content(test_pages[i]);
    // }
    // printk("wrote to all\r\n");
    // write_page(test_pages[0], "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA", 33);
    // write_page(test_pages[1], "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB", 33);
    // write_page(test_pages[2], "CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC", 33);

    // display_page_content(test_pages[0]);
    // display_page_content(test_pages[1]);
    // display_page_content(test_pages[2]);
    return;
}

void test_page_table()
{
    // dump_page_addresses();
    debug_allocator();
    return;
}



// tries to allocate a ton of pages
void test_heap_kmalloc()
{
    void** test_pages = (void*) kmalloc(sizeof(void*) * 35000); // aprox 32k pages avail in our system
    int i = 0;
    int max_page = 0;
    int bytes_to_allocate = 5000;
    printk("Starting Kmalloc stress test\r\n");
    print_heap();
    for(i=0; i < 35000; i++)
    {
        test_pages[i] = kmalloc(bytes_to_allocate);
        if(test_pages[i] == NULL)
        {
            printk(" ----> Was able to allocate %d pages", i);
            max_page = i;
            break;
        }
        printk("index: %d Address %p\r\n", i, test_pages[i]);
        if(i == 255)
        {
            printk("255\r\n");
        }
        
    }
    printk("Writing to the allocated pages\r\n");
    for(i=0; i < max_page; i++)
    {
        memset(test_pages[i], i, bytes_to_allocate);
    }
    printk("Sucessfully wrote to all the pages");
}

void test_heap()
{
    print_heap();
    test_heap_kmalloc();
    // void* test_ptrs[10000] = {0};
    // printk("why must it end this way?\r\n");
    print_heap();
    // test_ptrs[0] = kmalloc(32000); // order 3
    // // print_heap();
    // test_ptrs[1] = kmalloc(0x2000); //order 2
    // // print_heap();
    // test_ptrs[2] = kmalloc(420); // order 0
    // // print_heap();
    // test_ptrs[3] = kmalloc(420); // order 0
    // // print_heap();
    // test_ptrs[4] = kmalloc(420); // order 0
    // test_ptrs[5] = kmalloc(420); // order 0 // full heap
    // print_heap();
    // kfree(test_ptrs[4]);
    // // print_heap();
    // kfree(test_ptrs[5]);
    // print_heap();
    // kfree(test_ptrs[2]);
    // kfree(test_ptrs[3]);
    // print_heap();
    // kfree(test_ptrs[1]);
    // print_heap();
    // kfree(test_ptrs[0]);
    // // test_ptrs[6] = kmalloc(420); // should pass now
    // print_heap();
    // return;
}