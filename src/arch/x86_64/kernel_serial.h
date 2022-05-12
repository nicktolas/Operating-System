#ifndef KERNEL_SERIAL
#define KERNEL_SERIAL
#include "kernel.h"
#define SERIAL_BUFFER_SIZE 16

struct State
{
    char buff[SERIAL_BUFFER_SIZE];
    char *consumer;
    char *producer;
    int type;
    void* consumer_function;
}

// State Buffer Functions
void init_state(struct State *state);
void consumer_next(struct State *state);
int producer_add_char(char toAdd, struct State *state);
void consume_byte(char* byte);

// Serial Functions
void SER_init(void);
int SER_write(const char *buff, int len);
#endif