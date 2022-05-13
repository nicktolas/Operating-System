#ifndef KERNEL_SERIAL
#define KERNEL_SERIAL
#include "kernel.h"
#define SERIAL_BUFFER_SIZE 4096
#define SERIAL_OUT_PORT 0x3F8
#define SERIAL_IRR_REG_OFFSET 2
#define SERIAL_LINE_STATUS_REG_OFFSET 5
#define SERIAL_IRR_LINE_INT 0x6
#define SERIAL_IRR_TX_INT 0x2

struct State_Serial
{
    char buff[SERIAL_BUFFER_SIZE];
    char *consumer;
    char *producer;
};

// State Buffer Functions
void init_state_serial(struct State_Serial *state);
void consumer_next(struct State_Serial *state);
int producer_add_char(char toAdd, struct State_Serial *state);
void serial_consume(struct State_Serial *state);
int init_serial_device();
void serial_int_handler();

// Serial Functions
void SER_init(void);
int SER_write(const char *buff, int len);
#endif