#ifndef DOORS_STRING
#define DOORS_STRING
#define ITOA_BUF_SIZE 22
#define LTOA_BUF_SIZE 35
#define UTOA_BUF_SIZE 50
#define LLTOA_BUF_SIZE 80

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
#endif