#include "kernel.h"
#include "kernel_funcs.h"
#include "doors_keyboard.h"

void keyboard_init(void)
{
    struct PS_Controller_Config* curr_ps_config;

    // Send config to disable Port 1 and 2
    outb(PS2_DATA, PS2_CONTROLLER_PORT_FIRST_DISABLE);
    ps2_poll();
    outb(PS2_DATA, PS2_CONTROLLER_PORT_SECOND_DISABLE);
    // Enable clock and interupts of port 1. 
    curr_ps_config = (struct PS_Controller_Config*) inb(PS2_CONTROLLER_BYTE_ZERO);
    curr_ps_config->PS2_second_port_clk = 0;
    curr_ps_config->PS2_second_port_int = 0;
    curr_ps_config->PS2_first_port_clk = 1;
    curr_ps_config->PS2_first_port_int = 1;
    ps2_poll();
    outb(PS2_DATA, (char) curr_ps_config);
}

static char ps2_poll_read(void)
{
    char status = inb(PS2_STATUS);
    while (!(status & PS2_STATUS_OUTPUT))
    {
        status = inb(PS2_STATUS);
    }
    return inb(PS2_DATA);
}

void ps2_poll(void)
{
    char status = inb(PS2_STATUS);
    while (!(status & PS2_STATUS_OUTPUT))
    {
        status = inb(PS2_STATUS);
    }
    return;
}

static inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    asm volatile ( "inb %1, %0"
    : "=a"(ret)
    : "Nd"(port) );
    return ret;
}

static inline void outb(uint16_t port, uint8_t val)
{
    asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}