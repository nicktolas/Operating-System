# Script for generating the ISRs in assembly
error_codes = [8, 10, 11, 12, 13, 14, 17, 21, 29, 30]
asm_file = open("./src/arch/x86_64/int_handlers.asm", "w")
asm_file.write(".globl gen_isr_handler\n")
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
\tadd sub, #8 ; decrementing the stack pointer to account for a net line shift
\tmov rsi, rdi ; set value to the error code as arg
\tmov rdi, #{irq_num} ; set irq number as arg
\tbr gen_handler\n'''.format(irq_num = i))
    else:
        asm_file.write('''\tpush rsi ; save scratch registers -- consistent with above
\tpush rdi
\tmov rdi, #{irq_num} ; set irq number as arg
\tmov rsi, #69 ; set error code to a non random number
\tbr gen_handler\n'''.format(irq_num = i))
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