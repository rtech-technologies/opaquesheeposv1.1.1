section .text
bits 64

global _start
extern kmain
extern kstup

_start:
    mov rsp, stack_top
    push rdi
    call kstup
    pop rdi
    call kmain
.hang:
    hlt
    jmp .hang

section .bss
align 16
stack_bottom:
    resb 16384
stack_top:
