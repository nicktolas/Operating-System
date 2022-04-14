#include "kernel.h"
#include "kernel_memory.h"
#include "doors_string.h"

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