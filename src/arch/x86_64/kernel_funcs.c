#include "kernel.h"
#include "kernel_funcs.h"

static unsigned short *vgaBuff = (unsigned short*) K_VGA_BASE_ADDR;
static int vga_width = 80;
static int vga_height = 25;
static int vga_cursor_row = 0;
static int vga_cursor = 0;
static unsigned char vga_color = (K_VGA_COLOR_LGREY | (K_VGA_COLOR_BLACK << 4));


// ------------------------------- VGA Functions -----------------------------------

// Sets the color the VGA console is currently operating with
void set_VGA_color(unsigned int foreground, unsigned int background)
{
    vga_color = foreground | (background << 4);
    return;
}

// Prints the provided character to current position in the VGA console. 
void VGA_display_char(char c)
{
    if(c == '\n')
    {
        vga_cursor_row++;
        scroll_vga_window_up();
    }
    else if(c == '\r')
    {
        vga_cursor = 0;
    }
    else
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
// -------------------------- String Functions --------------------------------------

int printk(const char *fmt, ...)
{
    // int i;
    char* substring;
    va_list args_list;
    substring = strtok(fmt, '%');
    VGA_display_str(substring);
    substring = strtok(NULL, '%');
    while(substring != NULL)
    {
        // skip over first character (character after % delimiter)
        VGA_display_str(substring+1);
        // Call respective function (switch statement) substring[0]
        // switch (substring[0])
        // sets the substring and null terminates the string
        substring = strtok(NULL, '%');
    }
    va_end(args_list);
    return 0;
}
// } __attribute__ ((format (printf, 1, 2)));

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

// Returns the pointer to the first occurance of character c in the string str
const char* strchr(const char *str, int c)
{
    int i;
    for(i=0; i < strlen(str); i++)
    {
        if(str[i] == c)
        {
            return str+i;
        }
    }
    return NULL;
}

/* Copys n characters from string src to string dest.
If n exeeds the length of src, then function returns NULL.
n should not include the NULL byte that ends the string in src. 
This function will add the NULL byte at the end of dest.*/
char *strncpy(char *restrict dest, const char *restrict src, size_t n)
{
    if(n > strlen(src))
    {
        return NULL;
    }
    memcpy((void*)dest, (void*)src, n);
    dest[n] = '\0';
    return dest;
}


/* Different than the unistd strtok.
   str takes a given character pointer string and a character pointer delimiter and
   separates the string into null terminated substrings that are split by the delimiting character. 

   For reach all substrings of the split string, call strtok with the NULL parameter after the original strtok(provided string, delimit).
   Changing the delimiter after the intial call will continue off the next substring. 

   Note: This function modifies in place the provided string. All delmiited characters are replaced with a null terminating byte representing
   the end of the substring. The provided string should be within scope of the strtok function usage throughout its usage. This is to accomodate a no memory allocation requirement. 
*/
char* strtok(char *str, int delim)
{
    static char* token;
    char* found_char = NULL;
    char* return_token;
    // intial call
    if(str != NULL)
    {
        token = str;
    }
    found_char = strchr(token, delim);
    if(found_char == NULL)
    {
        token = NULL;
        return_token = NULL;
    }
    else
    {
        token[(int)(found_char-token)] = '\0';
        return_token = token;
        token = found_char+1; 
    }
    return return_token;
}

// --------------------------- Memory Manipulation Functions -----------------------------------

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

// // Copys n bytes of memory from dst to src. 
void *memcpy(void *restrict dest, const void *restrict src, size_t n)
{
    int i; 
    for(i=0; i < n; i++)
    {
        ((char*)dest)[i] = ((char*)src)[i];
    }
    return dest;
}