with_error:
    push rax ; save scratch registers
    push rbx
    mov rbx, [rsp + 16] ; save error code into rbx
    mov rax, [rsp + 8] ; save old rax into rax
    mov [rsp+16], rax ; move old rax into old error code spot
    mov rax, [rsp] ; move old rbx into rax
    mov [rsp+8], rax ; move old rbx up one position
    add sub, #8 ; decrementing the stack pointer to account for a net line shift
    mov rdi, #0 ; set irq number as arg
    mov rsi, rbx ; set value to the error code as arg
    br gen_handle

without_error:
    push rax ; save scratch registers -- consistent with above
    push rbx
    mov rdi, #1 ; set irq number as arg
    mov rsi, #69 ; set error code to a non random number
    br gen_handle

gen_handle:
    pop rax
    pop rbx
    push rbp ; push all caller save registers
    push rbx
    push r12
    push r13
    push r14
    push r15
    push rsp
    call gen_handler ; needs to be an extern that is global to the file

gen_return:
    ; pop rax ; this is the stack pointer
    ; pop r15
    ; pop r14
    ; pop r13
    ; pop r12
    ; pop rbx
    ; pop rbp
    ; pop rbx
    ; pop rax
    iretq