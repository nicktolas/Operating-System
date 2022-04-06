#include "kernel.h"
#include "kernel_funcs.h"

static unsigned short *vgaBuff = (unsigned short*) K_VGA_BASE_ADDR;
static int vga_width = 80;
// static int vga_height = 20;
static int vga_cursor = 0;
static unsigned char vga_color = (K_VGA_COLOR_LGREY | (K_VGA_COLOR_BLACK << 4));

// Sets the color the VGA console is currently operating with
void set_VGA_color(unsigned int foreground, unsigned int background)
{
    vga_color = foreground | (background << 4);
    return;
}

// Prints the provided character to current position in the VGA console. 
void VGA_display_char(char c)
{
    vgaBuff[vga_cursor] = (vga_color << 8) | c;
    if ((vga_cursor % vga_width) < (vga_width -1))
    {
        vga_cursor++;
    }
    return;
}

// Prints the provided characters in the string to the VGA console.
void VGA_display_str(const char *provided_string)
{
    int i;
    int length;
    length = strlen(provided_string);
    for(i=0;i<length-1;i++)
    {
        VGA_display_char(provided_string[i]);
    }
    return;
}

// Clears the VGA console completely
void VGA_clear(void)
{
    memset((void*) K_VGA_BASE_ADDR, 0, 4000);
    return;
}

//Sets the Doors OS background
void VGA_background(void)
{
    int i;
    int row = 0;
    int offset = 0;
    for(row = 0; row < 5; row++){
        set_VGA_color(K_VGA_COLOR_BLACK, K_VGA_COLOR_LCYAN);
        for(i=row*80; i< (row+1)*80; i++)
        {
            vgaBuff[i] = (vga_color << 8) | ' ';
        }
    }
    for(row = 5; row < 20; row++){
        set_VGA_color(K_VGA_COLOR_BLACK, K_VGA_COLOR_LCYAN);
        for(i=row*80; i< (row*80)+20; i++)
        {
            vgaBuff[i] = (vga_color << 8) | ' ';
        }
        offset = 20;
        if(row < 13)
        {
            set_VGA_color(K_VGA_COLOR_BLACK, K_VGA_COLOR_RED);
        }
        else
        {
            set_VGA_color(K_VGA_COLOR_BLACK, K_VGA_COLOR_BLUE);
        }
        for(i=row*80+offset; i< (row*80)+offset+19; i++)
        {
            vgaBuff[i] = (vga_color << 8) | ' ';
        }
        offset = 39;
        set_VGA_color(K_VGA_COLOR_BLACK, K_VGA_COLOR_LCYAN);
        for(i=row*80+offset; i< (row*80)+offset+2; i++)
        {
            vgaBuff[i] = (vga_color << 8) | ' ';
        }
        offset = 41;
        if(row < 13)
        {
            set_VGA_color(K_VGA_COLOR_BLACK, K_VGA_COLOR_GREEN);
        }
        else
        {
            set_VGA_color(K_VGA_COLOR_BLACK, K_VGA_COLOR_BROWN);
        }
        for(i=row*80+offset; i< (row*80)+offset+19; i++)
        {
            vgaBuff[i] = (vga_color << 8) | ' ';
        }
        offset = 60;
        set_VGA_color(K_VGA_COLOR_BLACK, K_VGA_COLOR_LCYAN);
        for(i=row*80+offset; i< (row+1)*80; i++)
        {
            vgaBuff[i] = (vga_color << 8) | ' ';
        }

    }
    for(row = 20; row < 25; row++){
        set_VGA_color(K_VGA_COLOR_BLACK, K_VGA_COLOR_LCYAN);
        for(i=row*80; i< (row+1)*80; i++)
        {
            vgaBuff[i] = (vga_color << 8) | ' ';
        }
    }
    set_VGA_color(K_VGA_COLOR_LGREY, K_VGA_COLOR_BLACK);
    return;
}

// Clears the specfied row (0-24) on the VGA console (25x80)
void VGA_clear_row(int row)
{
    // 
    short* base_addr = (short*) K_VGA_BASE_ADDR + (row*80);
    memset((void*) base_addr, 0, 80*2);
    return;
}

// Takes a string and returns a size_t representing then number of characters in the string excluding the terminating null byte.
size_t strlen(const char *provided_string)
{
    int length = 0;
    while(provided_string[length] != '\0')
    {
        length ++;
    }
    return length+1;
}

// Sets n bytes of memory to c starting at location defined by dst
void * memset(void *dst, int c, size_t n)
{
    int i;
    char* curr_pos = (char*)dst;
    for (i=0;i<n;i++)
    {
        curr_pos[i] = c;
        curr_pos++;
    }
    return dst; 
}
