#ifndef KERNEL_MEMORY
#define KERNEL_MEMORY
#include "kernel.h"
// Memory Manipulation
void * memset(void *dst, int c, size_t n);
void *memcpy(void *restrict dest, const void *restrict src, size_t n);

uint8_t inb(uint16_t port);
void outb(uint16_t port, uint8_t val);
#endif