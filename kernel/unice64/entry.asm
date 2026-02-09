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
    ; UEFI passes arguments in RCX, RDX, R8, R9 (Microsoft ABI)
    ; Kernel expects first argument in RDI (System V ABI)

    ; Setup our own stack
    mov rsp, stack_top

    ; Ensure 16-byte alignment and preserve binfo (RCX) for both calls
    push rcx     ; [rsp] = binfo, rsp = stack_top - 8
    sub rsp, 8   ; rsp = stack_top - 16 (16-byte aligned)

    ; Call the startup orchestrator
    mov rdi, rcx
    call kstup

    ; Call the main shell loop
    add rsp, 8   ; rsp = stack_top - 8
    pop rdi      ; rdi = binfo, rsp = stack_top
    call kmain

.hang:
    hlt
    jmp .hang

section .bss
align 16
stack_bottom:
    resb 16384
stack_top:
