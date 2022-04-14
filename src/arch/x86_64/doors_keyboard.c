#include "kernel.h"
#include "kernel_funcs.h"
#include "doors_keyboard.h"
#include "doors_string.h"

static bool pressed_list[300] = {0};
static bool capslock;
static bool shift; 
static bool secondary;

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
    capslock = false;
    shift = false; 
    secondary = false;
    while(hold)
    {
        byte_read = ps2_poll_read();
        parse_byte(byte_read);
    }
    return;
}

void parse_byte(uint8_t byte_read)
{
    switch(byte_read)
    {
        case F1_KEY:
            process_char(F1_KEY, "<F1>", shift, capslock);
            break;
        case F2_KEY:
            process_char(F2_KEY, "<F2>", shift, capslock);
            break;
        case F3_KEY:
            process_char(F3_KEY, "<F3>", shift, capslock);
            break;
        case F4_KEY:
            process_char(F4_KEY, "<F4>", shift, capslock);
            break;
        case F5_KEY:
            process_char(F5_KEY, "<F5>", shift, capslock);
            break;
        case F6_KEY:
            process_char(F6_KEY, "<F6>", shift, capslock);
            break;
        case F7_KEY:
            process_char(F7_KEY, "<F7>", shift, capslock);
            break;
        case F8_KEY:
            process_char(F8_KEY, "<F8>", shift, capslock);
            break;
        case F9_KEY:
            process_char(F9_KEY, "<F9>", shift, capslock);
            break;
        case F10_KEY:
            process_char(F10_KEY, "<F10>", shift, capslock);
            break;
        case F11_KEY:
            process_char(F11_KEY, "<F11>", shift, capslock);
            break;
        case F12_KEY:
            process_char(F12_KEY, "<F12>", shift, capslock);
            break;
        case _0_KEY:
            process_char(_0_KEY, "0", shift, capslock);
            break;
        case _1_KEY:
            process_char(_1_KEY, "1", shift, capslock);
            break;
        case _2_KEY:
            process_char(_2_KEY, "2", shift, capslock);
            break;
        case _3_KEY:
            process_char(_3_KEY, "3", shift, capslock);
            break;
        case _4_KEY:
            process_char(_4_KEY, "4", shift, capslock);
            break;
        case _5_KEY:
            process_char(_5_KEY, "5", shift, capslock);
            break;
        case _6_KEY:
            process_char(_6_KEY, "6", shift, capslock);
            break;
        case _7_KEY:
            process_char(_7_KEY, "7", shift, capslock);
            break;
        case _8_KEY:
            process_char(_8_KEY, "8", shift, capslock);
            break;
        case _9_KEY:
            process_char(_9_KEY, "9", shift, capslock);
            break;
        case MINUS_KEY:
            process_char(MINUS_KEY, "-", shift, capslock);
            break;
        case EQ_KEY:
            process_char(EQ_KEY, "=", shift, capslock);
            break;
        case SEMICOLON_KEY:
            process_char(SEMICOLON_KEY, ";", shift, capslock);
            break;
        case OBRACKET_KEY:
            process_char(OBRACKET_KEY, "[", shift, capslock);
            break;
        case CBRACKET_KEY:
            process_char(CBRACKET_KEY, "]", shift, capslock);
            break;
        case CAPSLOCK_KEY:
            if (pressed_list[CAPSLOCK_KEY])
            {
                pressed_list[CAPSLOCK_KEY] = false;
            }
            else
            {
                capslock = !capslock;      
            }
            break;
        case RSHIFT_KEY:
        case LSHIFT_KEY:
            if (pressed_list[LSHIFT_KEY])
            {
                pressed_list[LSHIFT_KEY] = false;
            }
            else
            {
                shift = !shift;      
            }
            break;
        case LCONTROL_KEY:
            process_char(LCONTROL_KEY, "<LCNTROL>", shift, capslock);
            break;
        case ENTER_KEY:
            process_char(ENTER_KEY, "<ENTER>", shift, capslock);
            break;
        case SPACE_KEY:
            process_char(SPACE_KEY, " ", shift, capslock);
            break;
        case TAB_KEY:
            process_char(TAB_KEY, "\t", shift, capslock);
            break;
        case BTICK_KEY:
            process_char(BTICK_KEY, "`", shift, capslock);
            break;
        case LALT_KEY:
            process_char(LALT_KEY, "<LALT>", shift, capslock);
            break;
        case ESCAPE_KEY:
            process_char(ESCAPE_KEY, "<ESCAPE>", shift, capslock);
            break;
        case NUMLOCK_KEY:
            process_char(NUMLOCK_KEY, "<NUMLOCK>", shift, capslock);
            break;
        case A_KEY:
            if(alternate_key())
            {
                process_char(A_KEY, "A", shift, capslock);
            }
            else
            {
                process_char(A_KEY, "a", shift, capslock);
            }
            
            break;
        case B_KEY:
            if(alternate_key())
            {
                process_char(B_KEY, "B", shift, capslock);
            }
            else
            {
                process_char(B_KEY, "b", shift, capslock);
            }
            break;
        case C_KEY:
            process_char(C_KEY, "c", shift, capslock);
            break;
        case D_KEY:
            process_char(D_KEY, "d", shift, capslock);
            break;
        case E_KEY:
            process_char(E_KEY, "e", shift, capslock);
            break;
        case F_KEY:
            process_char(F_KEY, "f", shift, capslock);
            break;
        case G_KEY:
            process_char(G_KEY, "g", shift, capslock);
            break;
        case H_KEY:
            process_char(H_KEY, "h", shift, capslock);
            break;
        case I_KEY:
            process_char(I_KEY, "i", shift, capslock);
            break;
        case J_KEY:
            process_char(J_KEY, "j", shift, capslock);
            break;
        case K_KEY:
            process_char(K_KEY, "k", shift, capslock);
            break;
        case L_KEY:
            process_char(L_KEY, "l", shift, capslock);
            break;
        case M_KEY:
            process_char(M_KEY, "m", shift, capslock);
            break;
        case N_KEY:
            process_char(N_KEY, "n", shift, capslock);
            break;
        case O_KEY:
            process_char(O_KEY, "o", shift, capslock);
            break;
        case P_KEY:
            process_char(P_KEY, "p", shift, capslock);
            break;
        case Q_KEY:
            process_char(Q_KEY, "q", shift, capslock);
            break;
        case R_KEY:
            process_char(R_KEY, "R", shift, capslock);
            break;
        case S_KEY:
            process_char(S_KEY, "s", shift, capslock);
            break;
        case T_KEY:
            process_char(T_KEY, "t", shift, capslock);
            break;
        case U_KEY:
            process_char(U_KEY, "u", shift, capslock);
            break;
        case V_KEY:
            process_char(V_KEY, "v", shift, capslock);
            break;
        case W_KEY:
            process_char(W_KEY, "w", shift, capslock);
            break;
        case X_KEY:
            process_char(X_KEY, "x", shift, capslock);
            break;
        case Y_KEY:
            process_char(Y_KEY, "y", shift, capslock);
            break;
        case Z_KEY:
            process_char(Z_KEY, "z", shift, capslock);
            break;
        case RELEASE_PRIMARY_SET:
            break;
        default:
            printk("\r\n Enountered unreocgnized character, Scan Code: %x\r\n", byte_read);
            break;
    }
    return;
}

// Prints the character decoded by the scancode. 
void process_char(uint8_t scancode, char* to_print, bool shift, bool capslock)
{
        if (pressed_list[scancode]) // if already pressed
        {
            pressed_list[scancode] = false;
        }
        else // print on press
        {
            printk("%s", to_print);
            pressed_list[scancode] = true;
        }
        return;
}

// Checks to see whether the alternate (shifted) key should be used. 
bool alternate_key(void)
{
    bool capitalize;
    capitalize = false;
    if(capslock)
    {
        capitalize = !capitalize;
    }
    if(shift)
    {
        capitalize = !capitalize;
    }
    return capitalize;
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