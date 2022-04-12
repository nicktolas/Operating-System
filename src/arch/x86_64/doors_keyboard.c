#include "kernel.h"
#include "kernel_funcs.h"
#include "doors_keyboard.h"
#include "doors_string.h"


void keyboard_init(void)
{
    struct PS_Controller_Config* curr_ps_config;
    uint8_t read_status;
    // Send config to disable Port 1 and 2
    outb(PS2_CMD, PS2_CONTROLLER_PORT_FIRST_DISABLE);
    // ps2_poll();
    outb(PS2_CMD, PS2_CONTROLLER_PORT_SECOND_DISABLE);
    outb(PS2_CMD, PS2_CONTROLLER_BYTE_ZERO);
    ps2_poll();
    // Enable clock and interupts of port 1.
    curr_ps_config = (struct PS_Controller_Config*) &read_status;
    read_status = inb(PS2_DATA);
    curr_ps_config->PS2_second_port_clk = 0;
    curr_ps_config->PS2_second_port_int = 0;
    curr_ps_config->PS2_first_port_clk = 0;
    curr_ps_config->PS2_first_port_int = 0;
    outb(PS2_CMD, PS2_DATA);
    outb(PS2_DATA, read_status);
    ps2_poll();
    outb(PS2_CMD, PS2_RESET);
    printk("Reset status: %x\r\n", ps2_poll_read());
    outb(PS2_DATA, SC2_PRESSED_F);
    // set scan 0x55 keep going
    outb(PS2_CMD, PS2_CONTROLLER_PORT_FIRST_ENABLE);
    return;
}

void keyboard_loop(void)
{
    int hold = 1;
    char byte_read;
    while(hold)
    {
        byte_read = ps2_poll_read();
        parse_byte(byte_read);
    }
    return;
}

void parse_byte(char byte_read)
{
    if(byte_read == SC2_PRESSED_F)
    {
        printk("f");
    }
    return;
}

char ps2_poll_read(void)
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

void ps2_poll_write(void)
{
    char status = inb(PS2_STATUS);
    while(!(status & PS2_STATUS_INPUT))
    {
        status = inb(PS2_STATUS);
    }
    return;
}

uint8_t inb(uint16_t port)
{
    uint8_t ret;
    asm volatile ( "inb %1, %0"
    : "=a"(ret)
    : "Nd"(port) );
    return ret;
}

void outb(uint16_t port, uint8_t val)
{
    asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
    return;
}