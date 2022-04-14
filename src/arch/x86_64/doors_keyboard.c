#include "kernel.h"
#include "kernel_funcs.h"
#include "doors_keyboard.h"
#include "doors_string.h"


void keyboard_init(void)
{
    struct PS_Controller_Config* curr_ps_config;
    uint8_t read_status;
    // Send config to disable Port 1 and 2
    ps2_write_CMD(PS2_CONTROLLER_PORT_FIRST_DISABLE);
    // ps2_poll();
    ps2_write_CMD(PS2_CONTROLLER_PORT_SECOND_DISABLE);
    // Flush the output buffer
    inb(PS2_DATA);
    // Get configuration information
    ps2_write_CMD(PS2_CONTROLLER_BYTE_ZERO);
    read_status = ps2_poll_read();
    // Enable clock and interupts of port 1.
    curr_ps_config = (struct PS_Controller_Config*) &read_status;
    curr_ps_config->PS2_second_port_clk = 1;
    curr_ps_config->PS2_second_port_int = 0;
    curr_ps_config->PS2_first_port_clk = 0;
    curr_ps_config->PS2_first_port_int = 0;
    curr_ps_config->PS2_port_translaton = 0;
    // Tell Controller to add this configuration
    ps2_write_CMD(PS2_DATA);
    // Wait for us to be able to write
    ps2_poll_write(read_status);
    // Test if Controller is functioning as intended
    ps2_write_CMD(PS2_CONTROLLER_SELF_TEST);
    read_status = ps2_poll_read();
    if(read_status == 0x55)
    {
        printk("PS2 Self Test: GOOD %x\r\n", read_status);
    }
    else
    {
        printk("PS2 Self Test: BAD %x\r\n", read_status);
    }
    // Enable the Controller
    ps2_write_CMD(PS2_CONTROLLER_PORT_FIRST_ENABLE);
    // Reset the keyboard device itself
    ps2_poll_write(PS2_RESET); // write to device
    // read the response
    printk("Reset status: %x\r\n", ps2_poll_read());
    // set scan 0x55 keep going
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
    static bool capslock;
    static bool shift;
    static bool release;
    static bool secondary;
    switch(byte_read)
    {
        case F1_KEY:
        case F2_KEY:
        case F3_KEY:
        case F4_KEY:
        case F5_KEY:
        case F6_KEY:
        case F7_KEY:
        case F8_KEY:
        case F9_KEY:
        case F10_KEY:
        case F11_KEY:
        case F12_KEY:
            printk("<Function Key>");
            secondary = false;
            break;
        default:
            printk("\r\n Enountered unreocgnized character, Scan Code: %x", byte_read);
            break;
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

void ps2_poll_write(char write_byte)
{
    char status = inb(PS2_STATUS);
    // if status buf bit 2 says input buf is empty (0)
    while((status & PS2_STATUS_INPUT)) // while input buf is full (1)
    {
        status = inb(PS2_STATUS);
    }
    outb(PS2_DATA, write_byte);
    return;
}

void ps2_write_CMD(char write_byte)
{
    char status = inb(PS2_STATUS);
    while((status & PS2_STATUS_INPUT))
    {
        status = inb(PS2_STATUS);
    }
    outb(PS2_CMD, write_byte);
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