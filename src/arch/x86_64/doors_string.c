#include "kernel.h"
#include "kernel_funcs.h"
#include "doors_string.h"

int printk(const char *fmt, ...)
{
    int i;
    va_list args_list;
    va_start(args_list, fmt);
    for(i=0; i < strlen(fmt); i++)
    {
        // format specfier - call respective function and double iterate the count
        if(fmt[i] == '%')
        {
            // No specfier and placed at end of the line
            if(i+1 >= strlen(fmt))
            {
                return -1;
            }
            i++; // iterate the index
            switch(fmt[i])
            {
                case '%': // %
                    VGA_display_char('%');
                    break;
                case 'd': // int
                    print_int((int) va_arg(args_list, int), 10);
                    break;
                case 'u': // unsigned decimal number
                    print_uint((unsigned int) va_arg(args_list, unsigned int), 10);
                    break;
                case 'x': // unsigned hex number
                    print_uint((unsigned int) va_arg(args_list, unsigned int), 16);
                    break;
                case 'c': // unsigned char
                    print_uchar((unsigned char) va_arg (args_list, int));
                    break;
                case 'p': // void* printed in hex
                    print_long_int(va_arg(args_list, long int), 16);
                    break;
                case 's': // const char *
                    print_str(va_arg(args_list, const char*));
                    break;
                case 'h': // short -- this is auto scaled to a int by library
                    i++;
                    if(fmt[i] == 'd') // short int
                    {
                        print_int((int) va_arg(args_list, int), 10);
                    }
                    else if(fmt[i] == 'u') // short unsigned int
                    {
                        print_uint((unsigned int) va_arg(args_list, unsigned int), 10);
                    }
                    else if(fmt[i] == 'x') // short hex
                    {
                        print_int((int) va_arg(args_list, int), 16);
                    }
                    break;
                case 'l': // long
                    i++;
                    if(fmt[i] == 'd') // long int
                    {
                        print_long_int((long int) va_arg(args_list, long int), 10);
                    }
                    else if(fmt[i] == 'u') // long unsigned int
                    {
                        print_ulong_int((unsigned long int) va_arg(args_list, unsigned long int), 10);
                    }
                    else if(fmt[i] == 'x') // long hex
                    {
                        print_long_int((long int) va_arg(args_list, long int), 16);
                    }
                    break;
                case 'q': // long long
                    i++;
                    if(fmt[i] == 'd') // int
                    {
                        print_llong_int((long long int) va_arg(args_list, long long int), 10);
                    }
                    else if(fmt[i] == 'u') // unsigned int
                    {
                        print_ullong_int((unsigned long long int) va_arg(args_list, unsigned long long int), 10);
                    }
                    else if(fmt[i] == 'x') // hex
                    {
                        print_llong_int((long long int) va_arg(args_list, long int), 16);
                    }
                    break;
                default:
                    VGA_display_str("\r\nERROR: Unknown printk option: ");
                    VGA_display_char(fmt[i]);
                    VGA_display_str("\r\n");
                    va_end(args_list);
                    return -1;
                    break;
            }
        }
        else
        {
            VGA_display_char(fmt[i]);
        }
    }
    va_end(args_list);
    return 0;
}
// } __attribute__ ((format (printf, 1, 2)));

// Prints an integer to the screen
void print_int(int i, int base)
{
    char string_of_int[ITOA_BUF_SIZE] = {0};
    if(base == 16)
    {
        print_hex_prefix();
    }
    VGA_display_str(itoa(i, string_of_int, base));
    return;
}

void print_uint(unsigned int i, int base)
{
    char string_of_int[UTOA_BUF_SIZE] = {0};
    if(base == 16)
    {
        print_hex_prefix();
    }
    VGA_display_str(uitoa(i, string_of_int, base));
    return;
}

void print_ulong_int(unsigned long int i, int base)
{
    char string_of_int[UTOA_BUF_SIZE] = {0};
    if(base == 16)
    {
        print_hex_prefix();
    }
    VGA_display_str(ultoa(i, string_of_int, base));
    return;
}

void print_long_int(long int i , int base)
{
    char string_of_int[LTOA_BUF_SIZE] = {0};
    if(base == 16)
    {
        print_hex_prefix();
    }
    VGA_display_str(ltoa(i, string_of_int, base));
    return;
}

void print_llong_int(long int i , int base)
{
    char string_of_int[LLTOA_BUF_SIZE] = {0};
    if(base == 16)
    {
        print_hex_prefix();
    }
    VGA_display_str(lltoa(i, string_of_int, base));
    return;
}

void print_ullong_int(long int i , int base)
{
    char string_of_int[LLTOA_BUF_SIZE] = {0};
    if(base == 16)
    {
        print_hex_prefix();
    }
    VGA_display_str(ulltoa(i, string_of_int, base));
    return;
}

void print_str(const char * provided_string)
{
    VGA_display_str(provided_string);
    return;
}

void print_uchar(unsigned char c)
{
    VGA_display_char(c);
    return;
}

void print_hex_prefix(void)
{
    VGA_display_char('0');
    VGA_display_char('x');
    return;
}

char* itoa(int provided_num, char* ret_buf, int base)
{
    char temp_buf[ITOA_BUF_SIZE] = {0};
    int val;
    int i;
    int j;
    i = 0;
    val = provided_num;
    // edge case for zero
    if(provided_num == 0)
    {
        ret_buf[0] = '0';
        ret_buf[1] = '\0';
        return ret_buf;
    }
    else if(provided_num < 0)
    {
        val = -provided_num;
    }
    while(val != 0)
    {
        // shift to letters, j is unused at this point
        j = val % base;
        if(j >= 10)
        {
            temp_buf[i] = 'a' + (j) - 10;
        }
        else
        {
            temp_buf[i] = '0' + (j);
        }
        val = val / base;
        i++;
    }
    if(provided_num < 0) // if a negative number add the sign
    {
        temp_buf[i] = '-';
        i++;
    }
    for(j=0; j < i; j++)
    {
        ret_buf[j] = temp_buf[i-j-1];
    }
    ret_buf[j] = '\0';
    return ret_buf;
}

char* uitoa(unsigned int provided_num, char* ret_buf, int base)
{
    char temp_buf[ITOA_BUF_SIZE] = {0};
    unsigned int val;
    int i;
    int j;
    i = 0;
    val = provided_num;
    // edge case for zero
    if(provided_num == 0)
    {
        ret_buf[0] = '0';
        ret_buf[1] = '\0';
        return ret_buf;
    }
    while(val != 0)
    {
        // shift to letters, j is unused at this point
        j = val % base;
        if(j >= 10)
        {
            temp_buf[i] = 'a' + (j) - 10;
        }
        else
        {
            temp_buf[i] = '0' + (j);
        }
        val = val / base;
        i++;
    }
    for(j=0; j < i; j++)
    {
        ret_buf[j] = temp_buf[i-j-1];
    }
    ret_buf[j] = '\0';
    return ret_buf;
}

char* ltoa(long int provided_num, char* ret_buf, int base)
{
    char temp_buf[LTOA_BUF_SIZE] = {0};
    long int val;
    int i;
    int j;
    i = 0;
    val = provided_num;
    // edge case for zero
    if(provided_num == 0)
    {
        ret_buf[0] = '0';
        ret_buf[1] = '\0';
        return ret_buf;
    }
    else if(provided_num < 0)
    {
        val = -provided_num;
    }
    while(val != 0)
    {
        // shift to letters, j is unused at this point
        j = val % base;
        if(j >= 10)
        {
            temp_buf[i] = 'a' + (j) - 10;
        }
        else
        {
            temp_buf[i] = '0' + (j);
        }
        val = val / base;
        i++;
    }
    if(provided_num < 0) // if a negative number add the sign
    {
        temp_buf[i] = '-';
        i++;
    }
    for(j=0; j < i; j++)
    {
        ret_buf[j] = temp_buf[i-j-1];
    }
    ret_buf[j] = '\0';
    return ret_buf;
}

char* ultoa(unsigned long int provided_num, char* ret_buf, int base)
{
    char temp_buf[ITOA_BUF_SIZE] = {0};
    unsigned long val;
    int i;
    int j;
    i = 0;
    val = provided_num;
    // edge case for zero
    if(provided_num == 0)
    {
        ret_buf[0] = '0';
        ret_buf[1] = '\0';
        return ret_buf;
    }
    while(val != 0)
    {
        // shift to letters, j is unused at this point
        j = val % base;
        if(j >= 10)
        {
            temp_buf[i] = 'a' + (j) - 10;
        }
        else
        {
            temp_buf[i] = '0' + (j);
        }
        val = val / base;
        i++;
    }
    for(j=0; j < i; j++)
    {
        ret_buf[j] = temp_buf[i-j-1];
    }
    ret_buf[j] = '\0';
    return ret_buf;
}

char* lltoa(long long int provided_num, char* ret_buf, int base)
{
    char temp_buf[LTOA_BUF_SIZE] = {0};
    long long int val;
    int i;
    int j;
    i = 0;
    val = provided_num;
    // edge case for zero
    if(provided_num == 0)
    {
        ret_buf[0] = '0';
        ret_buf[1] = '\0';
        return ret_buf;
    }
    else if(provided_num < 0)
    {
        val = -provided_num;
    }
    while(val != 0)
    {
        // shift to letters, j is unused at this point
        j = val % base;
        if(j >= 10)
        {
            temp_buf[i] = 'a' + (j) - 10;
        }
        else
        {
            temp_buf[i] = '0' + (j);
        }
        val = val / base;
        i++;
    }
    if(provided_num < 0) // if a negative number add the sign
    {
        temp_buf[i] = '-';
        i++;
    }
    for(j=0; j < i; j++)
    {
        ret_buf[j] = temp_buf[i-j-1];
    }
    ret_buf[j] = '\0';
    return ret_buf;
}

char* ulltoa(unsigned long long int provided_num, char* ret_buf, int base)
{
    char temp_buf[ITOA_BUF_SIZE] = {0};
    unsigned long long val;
    int i;
    int j;
    i = 0;
    val = provided_num;
    // edge case for zero
    if(provided_num == 0)
    {
        ret_buf[0] = '0';
        ret_buf[1] = '\0';
        return ret_buf;
    }
    while(val != 0)
    {
        // shift to letters, j is unused at this point
        j = val % base;
        if(j >= 10)
        {
            temp_buf[i] = 'a' + (j) - 10;
        }
        else
        {
            temp_buf[i] = '0' + (j);
        }
        val = val / base;
        i++;
    }
    for(j=0; j < i; j++)
    {
        ret_buf[j] = temp_buf[i-j-1];
    }
    ret_buf[j] = '\0';
    return ret_buf;
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
// char* strtok(char *str, int delim)
// {
//     static char* token;
//     char* found_char = NULL;
//     char* return_token;
//     // intial call
//     if(str != NULL)
//     {
//         token = str;
//     }
//     found_char = strchr(token, delim);
//     if(found_char == NULL)
//     {
//         token = NULL;
//         return_token = NULL;
//     }
//     else
//     {
//         token[(int)(found_char-token)] = '\0';
//         return_token = token;
//         token = found_char+1; 
//     }
//     return return_token;
// }