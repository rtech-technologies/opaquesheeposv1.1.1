section .text
bits 64

global _start
extern kmain
extern kstup

_start:
    ; Kernel Magic: 0x7f 'K' 'E' 'R'
    ; The bootloader verifies this and then jumps to _start + 4
    dd 0x52454b7f

    ; --- Actual Entry Point (_start + 4) ---
    ; Bootloader passes binfo in RDI (System V ABI)

    ; Clear interrupts just in case
    cli

    ; Setup our own stack
    mov rsp, stack_top

    ; Ensure 16-byte alignment and preserve binfo (RDI) for both calls
    push rdi     ; [rsp] = binfo, rsp = stack_top - 8
    sub rsp, 8   ; rsp = stack_top - 16 (16-byte aligned)

    ; Call the startup orchestrator
    ; RDI is already set to binfo
    call kstup

    ; Call the main shell loop
    add rsp, 8   ; rsp = stack_top - 8
    pop rdi      ; rdi = binfo, rsp = stack_top
    call kmain

.hang:
    hlt
    jmp .hang

section .note.GNU-stack noalloc noexec nowrite progbits

section .bss
align 16
stack_bottom:
    resb 16384
stack_top:
