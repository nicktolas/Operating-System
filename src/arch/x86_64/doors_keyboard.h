#ifndef DOORS_KEYBOARD
#define DOORS_KEYBOARD
#define PS2_DATA 0x60
#define PS2_CMD 0x64
#define PS2_STATUS PS2_CMD
#define PS2_STATUS_OUTPUT 1
#define PS2_STATUS_INPUT (1 << 1)
#define PS2_CONTROLLER_PORT_FIRST_DISABLE 0xAD
#define PS2_CONTROLLER_PORT_FIRST_ENABLE 0xAE
#define PS2_CONTROLLER_PORT_SECOND_DISABLE 0xA7
#define PS2_CONTROLLER_PORT_SECOND_ENABLE 0xA8
#define PS2_CONTROLLER_BYTE_ZERO 0x20

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

static inline uint8_t inb(uint16_t port);
static inline void outb(uint16_t port, uint8_t val);
static char ps2_poll_read(void);
void ps2_poll(void);
#endif