#include "kernel.h"
#include "kernel_memory.h"
#include "doors_string.h"
#include "kernel_interrupts.h"
#include "kernel_vga.h"
#include "kernel_serial.h"

static unsigned short *vgaBuff = (unsigned short*) K_VGA_BASE_ADDR;
static int vga_width = 80;
static int vga_height = 25;
static int vga_cursor_row = 0;
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
    CLI;
    if(c == '\n')
    {
        vga_cursor_row++;
        scroll_vga_window_up();
    }
    else if(c == '\r')
    {
        vga_cursor = 0;
    }
    else if(c == '\t')
    {
        VGA_display_char(' ');
        VGA_display_char(' ');
        VGA_display_char(' ');
    }
    else if (c != '\0')
    {
        vgaBuff[vga_cursor+(vga_cursor_row*80)] = (vga_color << 8) | c;
        if (vga_cursor < vga_width-1)
        {
            vga_cursor++;
        }
        else // vga cursor is at the end of the line
        { 
            vga_cursor_row++;
            vga_cursor = 0;
        }
    }
    STI_post_CLI;
    return;
}

// Scroll the window up if there is no room left to display on.
void scroll_vga_window_up()
{
    if (vga_cursor_row < vga_height)
    {
        return;
    }
    memcpy((void*)vgaBuff, (void*)(vgaBuff+vga_width), vga_width*(vga_height-1)*2);
    VGA_clear_row(vga_height-1);
    vga_cursor_row--;
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

// Clears the VGA console completely
void VGA_clear(void)
{
    int i;
    set_VGA_color(K_VGA_COLOR_LGREY, K_VGA_COLOR_BLACK);
    vga_cursor = 0;
    vga_cursor_row = 0;
    for(i=0; i < (vga_height * vga_width); i++)
    {
        VGA_display_char(' ');
    }
    vga_cursor = 0;
    vga_cursor_row = 0;
    return;
}

// Clears the specfied row (0-24) on the VGA console (25x80)
void VGA_clear_row(int row)
{
    int i;
    int curr_row;
    unsigned char old_color = vga_color;
    set_VGA_color(K_VGA_COLOR_LGREY, K_VGA_COLOR_BLACK);
    curr_row = vga_cursor_row;
    vga_cursor_row = row;
    vga_cursor = 0;

    for(i=0; i < (vga_width); i++)
    {
        VGA_display_char(' ');
    }
    vga_cursor = 0;
    vga_cursor_row = curr_row;
    vga_color = old_color;
    return;
}