section .multiboot_header
header_start:
    ; magic number
    dd 0xE85250D6
    dd 0
    dd 512
    dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start))
    ; type
    dw 0    
    ; flags
    dw 0    
    ; size
    dd 8
header_end:   