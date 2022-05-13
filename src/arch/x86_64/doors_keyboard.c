#include "kernel.h"
#include "kernel_memory.h"
#include "kernel_vga.h"
#include "doors_keyboard.h"
#include "doors_string.h"

static bool pressed_list[300] = {0};
static bool capslock;
static bool r_shift;
static bool l_shift;
static bool secondary;

void keyboard_init(void)
{
    struct PS_Controller_Config* curr_ps_config;
    uint8_t read_status;
    // Send config to disable Port 1 and 2
    ps2_write_CMD(PS2_CONTROLLER_PORT_FIRST_DISABLE);
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
    curr_ps_config->PS2_first_port_int = 1;
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
    capslock = false;
    l_shift = false; 
    r_shift = false;
    secondary = false;
    // ps2_poll_read();
    return;
}

// Loops the keybaord for processing input from keyboard and printing to screen.
void keyboard_loop(void)
{
    int hold = 1;
    char byte_read;
    capslock = false;
    l_shift = false; 
    r_shift = false;
    secondary = false;
    // consume garbage
    byte_read = ps2_poll_read();
    while(hold)
    {
        byte_read = ps2_poll_read();
        parse_byte(byte_read);
    }
    return;
}

// reads character from ps2 controller - called only from interrupt context
void keyboard_consume_byte(void)
{
    char byte_read = inb(PS2_DATA);
    parse_byte(byte_read);
}

void parse_byte(uint8_t byte_read)
{
    switch(byte_read)
    {
        case F1_KEY:
            process_char(F1_KEY, "<F1>");
            break;
        case F2_KEY:
            process_char(F2_KEY, "<F2>");
            break;
        case F3_KEY:
            process_char(F3_KEY, "<F3>");
            break;
        case F4_KEY:
            process_char(F4_KEY, "<F4>");
            break;
        case F5_KEY:
            process_char(F5_KEY, "<F5>");
            break;
        case F6_KEY:
            process_char(F6_KEY, "<F6>");
            break;
        case F7_KEY:
            process_char(F7_KEY, "<F7>");
            break;
        case F8_KEY:
            process_char(F8_KEY, "<F8>");
            break;
        case F9_KEY:
            process_char(F9_KEY, "<F9>");
            break;
        case F10_KEY:
            process_char(F10_KEY, "<F10>");
            break;
        case F11_KEY:
            process_char(F11_KEY, "<F11>");
            break;
        case F12_KEY:
            process_char(F12_KEY, "<F12>");
            break;
        case _0_KEY:
            if (alternate_key())
            {
                process_char(_0_KEY, ")");
            }
            else
            {
                process_char(_0_KEY, "0");
            }
            break;
        case _1_KEY:
            if (alternate_key())
            {
                process_char(_1_KEY, "!");
            }
            else
            {
                process_char(_1_KEY, "1");
            }
            break;
        case _2_KEY:
            if (alternate_key())
            {
                process_char(_2_KEY, "@");
            }
            else
            {
                process_char(_2_KEY, "2");
            }
            break;
        case _3_KEY:
            if (alternate_key())
            {
                process_char(_3_KEY, "#");
            }
            else
            {
                process_char(_3_KEY, "3");
            }
            break;
        case _4_KEY:
            if (alternate_key())
            {
                process_char(_4_KEY, "$");
            }
            else
            {
                process_char(_4_KEY, "4");
            }
            break;
        case _5_KEY:
            if (alternate_key())
            {
                process_char(_5_KEY, "%%");
            }
            else
            {
                process_char(_5_KEY, "5");
            }
            break;
        case _6_KEY:
            if (alternate_key())
            {
                process_char(_6_KEY, "^");
            }
            else
            {
                process_char(_6_KEY, "6");
            }
            break;
        case _7_KEY:
            if (alternate_key())
            {
                process_char(_7_KEY, "&");
            }
            else
            {
                process_char(_7_KEY, "7");
            }
            break;
        case _8_KEY:
            if (alternate_key())
            {
                process_char(_8_KEY, "*");
            }
            else
            {
                process_char(_8_KEY, "8");
            }
            break;
        case _9_KEY:
            if (alternate_key())
            {
                process_char(_9_KEY, "(");
            }
            else
            {
                process_char(_9_KEY, "9");
            }
            break;
        case MINUS_KEY:
            if (alternate_key())
            {
                process_char(MINUS_KEY, "_");
            }
            else
            {
                process_char(MINUS_KEY, "-");
            }
            break;
        case EQ_KEY:
            if (alternate_key())
            {
                process_char(EQ_KEY, "+");
            }
            else
            {
                process_char(EQ_KEY, "=");
            }
            break;
        case SEMICOLON_KEY:
            if (alternate_key())
            {
                process_char(SEMICOLON_KEY, ":");
            }
            else
            {
                process_char(SEMICOLON_KEY, ";");
            }
            break;
        case OBRACKET_KEY:
            if (alternate_key())
            {
                process_char(OBRACKET_KEY, "{");
            }
            else
            {
                process_char(OBRACKET_KEY, "[");
            }
            break;
        case CBRACKET_KEY:
            if (alternate_key())
            {
                process_char(CBRACKET_KEY, "}");
            }
            else
            {
                process_char(CBRACKET_KEY, "]");
            }
            break;
        case CAPSLOCK_KEY:
            if (pressed_list[CAPSLOCK_KEY])
            {
                pressed_list[CAPSLOCK_KEY] = false;
            }
            else
            {
                capslock = !capslock; 
                pressed_list[CAPSLOCK_KEY] = true; //pressed
            }
            break;
        case RSHIFT_KEY:
            r_shift = !r_shift;
            break;
        case LSHIFT_KEY:
            l_shift = !l_shift;
            break;
        case LCONTROL_KEY:
            process_char(LCONTROL_KEY, "<LCNTROL>");
            break;
        case ENTER_KEY:
            process_char(ENTER_KEY, "<ENTER>");
            break;
        case SPACE_KEY:
            process_char(SPACE_KEY, " ");
            break;
        case TAB_KEY:
            process_char(TAB_KEY, "\t");
            break;
        case BTICK_KEY:
            if (alternate_key())
            {
                process_char(BTICK_KEY, "~");
            }
            else
            {
                process_char(BTICK_KEY, "`");
            }
            break;
        case COMMA_KEY:
            if (alternate_key())
            {
                process_char(COMMA_KEY, "<");
            }
            else
            {
                process_char(COMMA_KEY, ",");
            }
            break;
        case PERIOD_KEY:
            if (alternate_key())
            {
                process_char(PERIOD_KEY, ">");
            }
            else
            {
                process_char(PERIOD_KEY, ".");
            }
            break;
        case FSLASH_KEY:
            if (alternate_key())
            {
                process_char(FSLASH_KEY, "?");
            }
            else
            {
                process_char(FSLASH_KEY, "/");
            }
            break;
        case BSLASH_KEY:
            if (alternate_key())
            {
                process_char(BSLASH_KEY, "|");
            }
            else
            {
                process_char(BSLASH_KEY, "\\");
            }
            break;
        case APOSTROPHE_KEY:
            if (alternate_key())
            {
                process_char(APOSTROPHE_KEY, "\"");
            }
            else
            {
                process_char(APOSTROPHE_KEY, "\'");
            }
            break;
        case LALT_KEY:
            process_char(LALT_KEY, "<LALT>");
            break;
        case ESCAPE_KEY:
            process_char(ESCAPE_KEY, "<ESCAPE>");
            break;
        case NUMLOCK_KEY:
            process_char(NUMLOCK_KEY, "<NUMLOCK>");
            break;
        case A_KEY:
            if(alternate_key())
            {
                process_char(A_KEY, "A");
            }
            else
            {
                process_char(A_KEY, "a");
            }
            
            break;
        case B_KEY:
            if(alternate_key())
            {
                process_char(B_KEY, "B");
            }
            else
            {
                process_char(B_KEY, "b");
            }
            break;
        case C_KEY:
            if (alternate_key())
            {
                process_char(C_KEY, "C");
            }
            else
            {
                process_char(C_KEY, "c");
            }
            break;
        case D_KEY:
            if (alternate_key())
            {
                process_char(D_KEY, "D");
            }
            else
            {
                process_char(D_KEY, "d");
            }
            break;
        case E_KEY:
            if (alternate_key())
            {
                process_char(E_KEY, "E");
            }
            else
            {
                process_char(E_KEY, "e");
            }
            break;
        case F_KEY:
            if (alternate_key())
            {
                process_char(F_KEY, "F");
            }
            else
            {
                process_char(F_KEY, "f");
            }
            break;
        case G_KEY:
            if (alternate_key())
            {
                process_char(G_KEY, "G");
            }
            else
            {
                process_char(G_KEY, "g");
            }
            break;
        case H_KEY:
            if (alternate_key())
            {
                process_char(H_KEY, "H");
            }
            else
            {
                process_char(H_KEY, "h");
            }
            break;
        case I_KEY:
            if (alternate_key())
            {
                process_char(I_KEY, "I");
            }
            else
            {
                process_char(I_KEY, "i");
            }
            break;
        case J_KEY:
            if (alternate_key())
            {
                process_char(J_KEY, "J");
            }
            else
            {
                process_char(J_KEY, "j");
            }
            break;
        case K_KEY:
            if (alternate_key())
            {
                process_char(K_KEY, "K");
            }
            else
            {
                process_char(K_KEY, "k");
            }
            break;
        case L_KEY:
            if (alternate_key())
            {
                process_char(L_KEY, "L");
            }
            else
            {
                process_char(L_KEY, "l");
            }
            break;
        case M_KEY:
            if (alternate_key())
            {
                process_char(M_KEY, "M");
            }
            else
            {
                process_char(M_KEY, "m");
            }
            break;
        case N_KEY:
            if (alternate_key())
            {
                process_char(N_KEY, "N");
            }
            else
            {
                process_char(N_KEY, "n");
            }
            break;
        case O_KEY:
            if (alternate_key())
            {
                process_char(O_KEY, "O");
            }
            else
            {
                process_char(O_KEY, "o");
            }
            break;
        case P_KEY:
            if (alternate_key())
            {
                process_char(P_KEY, "P");
            }
            else
            {
                process_char(P_KEY, "p");
            }
            break;
        case Q_KEY:
            if (alternate_key())
            {
                process_char(Q_KEY, "Q");
            }
            else
            {
                process_char(Q_KEY, "q");
            }
            break;
        case R_KEY:
            if (alternate_key())
            {
                process_char(R_KEY, "R");
            }
            else
            {
                process_char(R_KEY, "r");
            }
            break;
        case S_KEY:
            if (alternate_key())
            {
                process_char(S_KEY, "S");
            }
            else
            {
                process_char(S_KEY, "s");
            }
            break;
        case T_KEY:
            if (alternate_key())
            {
                process_char(T_KEY, "T");
            }
            else
            {
                process_char(T_KEY, "t");
            }
            break;
        case U_KEY:
            if (alternate_key())
            {
                process_char(U_KEY, "U");
            }
            else
            {
                process_char(U_KEY, "u");
            }
            break;
        case V_KEY:
            if (alternate_key())
            {
                process_char(V_KEY, "V");
            }
            else
            {
                process_char(V_KEY, "v");
            }
            break;
        case W_KEY:
            if (alternate_key())
            {
                process_char(W_KEY, "W");
            }
            else
            {
                process_char(W_KEY, "w");
            }
            break;
        case X_KEY:
            if (alternate_key())
            {
                process_char(X_KEY, "X");
            }
            else
            {
                process_char(X_KEY, "x");
            }
            break;
        case Y_KEY:
            if (alternate_key())
            {
                process_char(Y_KEY, "Y");
            }
            else
            {
                process_char(Y_KEY, "y");
            }
            break;
        case Z_KEY:
            if (alternate_key())
            {
                process_char(Z_KEY, "Z");
            }
            else
            {
                process_char(Z_KEY, "z");
            }
            break;
        case RELEASE_PRIMARY_SET:
            break;
        
        case 0xAA:
            break;
        
        default:
            printk("\r\n Enountered unreocgnized character, Scan Code: %x\r\n", byte_read);
            break;
    }
    return;
}

// Prints the character decoded by the scancode. 
void process_char(uint8_t scancode, char* to_print)
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
    if(l_shift || r_shift)
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