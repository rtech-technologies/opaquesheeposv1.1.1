section .text
bits 64

global _start
extern kmain
extern kstup
extern iolib_init

_start:
    mov rsp, stack_top
    call iolib_init
    call kstup
    call kmain
.hang:
    hlt
    jmp .hang

section .bss
align 16
stack_bottom:
    resb 16384
stack_top:
