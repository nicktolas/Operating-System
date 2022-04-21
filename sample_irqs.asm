.globl gen_isr_handler
with_error:
    push rsi ; save scratch registers
    push rdi
    mov rdi, [rsp + 16] ; save error code into rbx
    mov rsi, [rsp + 8] ; save old rax into rax
    mov [rsp+16], rsi ; move old rax into old error code spot
    mov rsi, [rsp] ; move old rbx into rax
    mov [rsp+8], rsi ; move old rbx up one position
    add sub, #8 ; decrementing the stack pointer to account for a net line shift
    mov rsi, rdi ; set value to the error code as arg
    mov rdi, #0 ; set irq number as arg
    br gen_handler

without_error:
    push rsi ; save scratch registers -- consistent with above
    push rdi
    mov rdi, #1 ; set irq number as arg
    mov rsi, #69 ; set error code to a non random number
    br gen_handler

gen_handler:
    push rsp
    push rbp ; push all caller save registers
    push rbx
    push r12
    push r13
    push r14
    push r15
    call gen_isr_handler ; needs to be an extern that is global to the file

gen_return:
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rsp
    pop rdi
    pop rsi
    iretq