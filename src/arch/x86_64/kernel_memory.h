#ifndef KERNEL_MEMORY
#define KERNEL_MEMORY

// Memory Manipulation
void * memset(void *dst, int c, size_t n);
void *memcpy(void *restrict dest, const void *restrict src, size_t n);

#endif