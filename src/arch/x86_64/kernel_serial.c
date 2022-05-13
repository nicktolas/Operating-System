#include "kernel_serial.h"
#include "kernel_memory.h"
#include "kernel_interrupts.h"

struct State_Serial serial_state;

/* Initalizes the State_Serial structure by setting producer and consumer to 0*/
void init_state_serial(struct State_Serial *state)
{
    state->consumer = &state->buff[0];
    state->producer = &state->buff[0];
    state->avail_slots = SERIAL_BUFFER_SIZE-1;
    return;
}

/* Initializes the Serial Device
   Only use once state structure is operational and setup */
int init_serial_device()
{
    outb(SERIAL_OUT_PORT + 1, 0x00);    // Disable all interrupts
    outb(SERIAL_OUT_PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    outb(SERIAL_OUT_PORT + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
    outb(SERIAL_OUT_PORT + 1, 0x00);    //                  (hi byte)
    outb(SERIAL_OUT_PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
    outb(SERIAL_OUT_PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    outb(SERIAL_OUT_PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
    outb(SERIAL_OUT_PORT + 4, 0x1E);    // Set in loopback mode, test the serial chip
    outb(SERIAL_OUT_PORT + 0, 0xAE);    // Test serial chip (send byte 0xAE and check if serial returns same byte)
 
   // Check if serial is faulty (i.e: not same byte as sent)
    if(inb(SERIAL_OUT_PORT + 0) != 0xAE)
    {
        asm("hlt;");
        return 1;
    }
 
   // If serial is not faulty set it in normal operation mode
   // (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
    outb(SERIAL_OUT_PORT + 4, 0x0F);

    // interrupt enable
    outb(SERIAL_OUT_PORT + 1, 0x06); // 110
    return 0;
}


/* Iterates the consider pointer and consumes the byte */
void consumer_next(struct State_Serial *state)
{
    CLI;
    if(state->consumer == state->producer)
    {
        // need to clear the interrupt still
        STI_post_CLI;
        return;
    }
    serial_consume(state);
    state->avail_slots ++;
    // serial write
    state->consumer++;
    if(state->consumer >= &state->buff[SERIAL_BUFFER_SIZE])
    {
        state->consumer = &state->buff[0];
    }
    STI_post_CLI;
}

/* Adds a character to the circular buffer and iterates the producer pointer */
int producer_add_char(char toAdd, struct State_Serial *state)
{
    CLI;
    /* if adding char would set both pointers equal */
    if(state->producer == state->consumer - 1 || \
    (state->consumer == &state->buff[0] && state->producer == &state->buff[SERIAL_BUFFER_SIZE-1]) )
    {
        STI_post_CLI;
        return 0;
    }
    *state->producer++ = toAdd;
    state->avail_slots--;
    if(state->producer >= &state->buff[SERIAL_BUFFER_SIZE])
    {
        state->producer = &state->buff[0];
    }
    STI_post_CLI;
    return 0;
   
}

/* Consumes the byte and sends it to its responsible function
   Only ever called while interrupts are off */
void serial_consume(struct State_Serial *state)
{
    char out_char = state->consumer[0];
    outb(SERIAL_OUT_PORT,out_char);
    return;
}

/* Interrupt Handler used in the general ISR handler. Cannot block.
   When serial interrupt is raised, we need to determine cause of interrupt.
   If Line interrupt, LSR is cleared. If TX interupt we write and set to idle. */
void serial_int_handler()
{
    uint8_t read_byte = inb(SERIAL_OUT_PORT + SERIAL_IRR_REG_OFFSET);
    if((read_byte & SERIAL_IRR_LINE_INT) == SERIAL_IRR_LINE_INT) // line reg int
    {
        inb(SERIAL_OUT_PORT + SERIAL_LINE_STATUS_REG_OFFSET); // Reading LSR clears
    }
    else if((read_byte & SERIAL_IRR_TX_INT) == SERIAL_IRR_TX_INT) // transmitter int
    {
        consumer_next(&serial_state); // writing THR clears int
    }
    return;
}

/* Initializes the serial IO and state structure */
void SER_init(void)
{
    init_state_serial(&serial_state);
    init_serial_device();
    return;
}

/* Writes a string of provided length to serial.
   This is a interrupt based process. Will not be instant. */
int SER_write(const char *buff, int len)
{
    CLI; // needs locking to prevent avail_slots race conditions
    int i;
    if (len >= serial_state.avail_slots)
    {
        STI_post_CLI;
        return -1;
    }
    for (i=0; i<len; i++) // TODO: account for when someone writes something too large. 
    {
        producer_add_char(buff[i], &serial_state);
    }
    STI_post_CLI;
    return 0;
}