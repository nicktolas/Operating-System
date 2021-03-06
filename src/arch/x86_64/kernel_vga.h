#ifndef KERNEL_VGA
#define KERNEL_VGA
#define K_VGA_BASE_ADDR 0xb8000
#define K_VGA_COLOR_BLACK 0
#define K_VGA_COLOR_BLUE 1
#define K_VGA_COLOR_GREEN 2
#define K_VGA_COLOR_CYAN 3
#define K_VGA_COLOR_RED 4
#define K_VGA_COLOR_MAGENTA 5
#define K_VGA_COLOR_BROWN 6
#define K_VGA_COLOR_LGREY 7
#define K_VGA_COLOR_DGREY 8
#define K_VGA_COLOR_LBLUE 9
#define K_VGA_COLOR_LGREEN 10
#define K_VGA_COLOR_LCYAN 11
#define K_VGA_COLOR_LRED 12
#define K_VGA_COLOR_LMAGENTA 13
#define K_VGA_COLOR_LBROWN 14
#define K_VGA_COLOR_WHITE 15

void VGA_set_color(int foreground, int background);
void VGA_display_char(char c);
void VGA_display_str(const char *provided_string);
void VGA_clear(void);
void VGA_background(void);
void VGA_clear_row(int row);
void scroll_vga_window_up(void);

#endif