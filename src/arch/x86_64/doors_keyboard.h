#ifndef DOORS_KEYBOARD
#include "scan_codes.h"
#define DOORS_KEYBOARD
#define PS2_DATA 0x60
#define PS2_CMD 0x64
#define PS2_RESET 0xFF
#define PS2_STATUS PS2_CMD
#define PS2_STATUS_OUTPUT 1
#define PS2_STATUS_INPUT (1 << 1)
#define PS2_CONTROLLER_PORT_FIRST_DISABLE 0xAD
#define PS2_CONTROLLER_PORT_FIRST_ENABLE 0xAE
#define PS2_CONTROLLER_PORT_SECOND_DISABLE 0xA7
#define PS2_CONTROLLER_PORT_SECOND_ENABLE 0xA8
#define PS2_CONTROLLER_BYTE_ZERO 0x20
#define PS2_CONTROLLER_SELF_TEST 0xAA
#define PS2_CONTROLLER_WRITE_NEXT 0xD1
#define SC2_PRESSED_F 0x2B

/*
    Struct for configuring the PS/2 Controller. 
    PS2_zero_one/two must be set to zero by user.
*/
struct PS_Controller_Config
{
    uint8_t PS2_first_port_int:1;
    uint8_t PS2_second_port_int:1;
    uint8_t PS2_sys_flag:1;
    uint8_t PS2_zero_one:1;
    uint8_t PS2_first_port_clk:1;
    uint8_t PS2_second_port_clk:1;
    uint8_t PS2_port_translaton:1;
    uint8_t PS2_zero_two:1;
}__attribute__((packed));

void process_char(uint8_t scancode, char* to_print);
bool alternate_key(void);


char ps2_poll_read(void);
void ps2_poll(void);
void ps2_poll_write(char write_byte);
void ps2_write_CMD(char write_byte);
void keyboard_init(void);
void keyboard_loop(void);
void parse_byte(uint8_t byte_read);


#endif