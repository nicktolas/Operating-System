#ifndef KERNEL_FUNCS
#define KERNEL_FUNCS
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
#define ITOA_BUF_SIZE 22
#define LTOA_BUF_SIZE 35
#define UTOA_BUF_SIZE 50
#define LLTOA_BUF_SIZE 80

//  VGA functions
void VGA_set_color(int foreground, int background);
void VGA_display_char(char c);
void VGA_display_str(const char *provided_string);
void VGA_clear(void);
void VGA_background(void);
void VGA_clear_row(int row);
void scroll_vga_window_up(void);


// String Functions
int printk(const char *fmt, ...) __attribute__ ((format (printf, 1, 2)));
void print_int(int i, int base);
void print_uint(unsigned int i, int base);
void print_long(long int i, int base);
void print_char(char c);
void print_str(const char * provided_string);
void print_uchar(unsigned char c);
void print_long_int(long int i , int base);
void print_ulong_int(unsigned long int i, int base);
void print_llong_int(long int i , int base);
void print_ullong_int(long int i , int base);
void print_hex_prefix(void);
char* itoa(int provided_num, char* ret_buf, int base);
char* ltoa(long int provided_num, char* ret_buf, int base);
char* uitoa(unsigned  int provided_num, char* ret_buf, int base);
char* ultoa(unsigned long int provided_num, char* ret_buf, int base);
char* lltoa(long long int provided_num, char* ret_buf, int base);
char* ulltoa(unsigned long long int provided_num, char* ret_buf, int base);
size_t strlen(const char *provided_string);
const char* strchr(const char *str, int c);
char* strtok(char *str, int delim);
char *strncpy(char *restrict dest, const char *restrict src, size_t n);

// Memory Manipulation
void * memset(void *dst, int c, size_t n);
void *memcpy(void *restrict dest, const void *restrict src, size_t n);

#endif