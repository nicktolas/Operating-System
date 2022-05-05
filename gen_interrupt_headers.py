#  Script for generating the ISRs in assembly
error_codes = [8, 10, 11, 12, 13, 14, 17, 21, 29, 30]
asm_file = open("./src/arch/x86_64/int_handlers.asm", "w")
asm_file.write("extern gen_isr_handler\n")
for i in range(0,256):
    asm_file.write("global asm_isr_{irq_num}\n".format(irq_num = i))
for i in range(0,256):
    asm_file.write("asm_isr_{irq_num}:\n".format(irq_num = i))
    if i in error_codes:
        asm_file.write('''\tpush rsi ; save scratch registers
\tpush rdi
\tmov rdi, [rsp + 16] ; save error code into rbx
\tmov rsi, [rsp + 8] ; save old rax into rax
\tmov [rsp+16], rsi ; move old rax into old error code spot
\tmov rsi, [rsp] ; move old rbx into rax
\tmov [rsp+8], rsi ; move old rbx up one position
\tadd rsp, 8 ; decrementing the stack pointer to account for a net line shift
\tmov rsi, rdi ; set value to the error code as arg
\tmov rdi, {irq_num} ; set irq number as arg
\tjmp gen_handler\n'''.format(irq_num = i))
    else:
        asm_file.write('''\tpush rsi ; save scratch registers -- consistent with above
\tpush rdi
\tmov rdi, {irq_num} ; set irq number as arg
\tmov rsi, 69 ; set error code to a non random number
\tjmp gen_handler\n'''.format(irq_num = i))
asm_file.write('''
gen_handler:
\tpush rsp
\tpush rbp ; push all caller save registers
\tpush rbx
\tpush r12
\tpush r13
\tpush r14
\tpush r15
\tcall gen_isr_handler ; needs to be an extern that is global to the file

gen_return:
\tpop r15
\tpop r14
\tpop r13
\tpop r12
\tpop rbx
\tpop rsp
\tpop rdi
\tpop rsi
\tiretq''')

asm_file.close()

#  Header file of isrs
c_header = open("./src/arch/x86_64/isr_func_headers.h", "w")
c_header.write("#ifndef ISR_EXTERNS\n")
c_header.write("#define ISR_EXTERNS\n")
for i in range(0, 256):
    c_header.write("extern void *asm_isr_{isr_num};\n".format(isr_num = i))
c_header.write("#endif")
c_header.close()

#  big c function - not explicilty needed
# c_func = open("isr_occupy.c", "w")
# c_func.write("switch(isr_value)\n{\n")
# for i in range(0, 256):
#     c_func.write("\tcase {isr_num}:\n".format(isr_num = i))
#     c_func.write("\t\tbreak;\n")
# c_func.write("}\n")
# c_func.close()

#  occupy_idt c function
c_func = open("idt_assign.c", "w")
for i in range(0, 256):
    c_func.write('''Int_Desc_Table_Entries[{irq_num}].zero = 0;
Int_Desc_Table_Entries[{irq_num}].present = 1;
Int_Desc_Table_Entries[{irq_num}].one = 1;
Int_Desc_Table_Entries[{irq_num}].protection_level = 0;
Int_Desc_Table_Entries[{irq_num}].stack_target = 0;
Int_Desc_Table_Entries[{irq_num}].int_trap_gate = CGD_TRAP;
Int_Desc_Table_Entries[{irq_num}].target_selector = 8;
Int_Desc_Table_Entries[{irq_num}].target_offset_top = ((uint64_t) &asm_isr_{irq_num}) >> 32;
Int_Desc_Table_Entries[{irq_num}].target_offset_mid = (((uint64_t) &asm_isr_{irq_num}) & MASK_BITS_L32) >> 16;
Int_Desc_Table_Entries[{irq_num}].target_offset_bot = (((uint64_t) &asm_isr_{irq_num}) & MASK_BITS_L16);\n'''.format(irq_num = i))
c_func.close()