section .text
bits 64

global _start
extern kmain
extern STup

_start:
    ; --- Kernel Entry Point ---
    ; Follows UEFI x64 ABI (Microsoft Convention)
    ; Bootloader passes binfo pointer in RCX

    ; Setup our own stack
    lea rsp, [rel stack_top]

    ; Bridge ABI: Move RCX (Microsoft) to RDI (System V) for kernel C functions
    mov rdi, rcx

    ; Ensure 16-byte alignment and preserve binfo (RDI) for both calls
    push rdi     ; [rsp] = binfo, rsp = stack_top - 8
    sub rsp, 8   ; rsp = stack_top - 16 (16-byte aligned)

    ; Call the startup orchestrator
    ; RDI is already set to binfo
    call STup

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
