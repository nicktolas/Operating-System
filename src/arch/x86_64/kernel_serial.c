#include "kernel_serial.h"
#include "kernel_memory.h"

/* Initalizes the State structure by setting producer and consumer to 0*/
void init_state(struct State *state)
{
    state->consumer = &state->buff[0];
    state->producer = &state->buff[0];
    return;
}

/* Iterates the consider pointer and consumes the byte*/
void consumer_next(struct State *state)
{
    if(state->consumer == state->producer)
    {
        return;
    }
    // serial write
    state->consumer++;
    if(state->consumer >= &state->buff[SERIAL_BUFFER_SIZE])
    {
        state->consumer = &state->buff[0];
    }
}

/* Adds a character to the circular buffer and iterates the producer pointer */
int producer_add_char(char toAdd, struct State *state)
{
    if(state->producer == state->consumer - 1 || \
    (state->consumer == &state->buff[0] && state->producer == &state->buff[SERIAL_BUFFER_SIZE-1]) )
    {
        return 0;
    }
    *state->producer++ = toAdd;
    if(state->producer >= &state->buff[SERIAL_BUFFER_SIZE])
    {
        state->producer = &state->buff[0];
    }
    return 0;
}

/* Consumes the byte and sends it to its responsible function*/
void consume_byte(char* byte)
{
    return;
}

/* Initializes the serial IO*/
void SER_init(void)
{
    return;
}

/* Writes a char array to the buffer*/
int SER_write(const char *buff, int len)
{
    return 0;
}