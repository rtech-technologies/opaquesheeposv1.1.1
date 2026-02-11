.code64
.intel_syntax noprefix

.extern kstup
.extern keyboard_handler_c
.extern disk_handler_c
.global _start
.global isr1
.global isr14

.section .text.prologue
_start:
    # 1. Set up a real stack (Kernel won't work without this!)
    lea rsp, [kernel_stack_top]

    # 2. Pass the BootParams pointer (RCX is usually the first arg in UEFI)
    # rdi = rcx (System V calling convention for the kernel)
    mov rdi, rcx

    # 3. Jump to the C startup
    call kstup

    # 4. If kstup returns, halt the CPU
.halt:
    hlt
    jmp .halt

isr1:
    push rax
    push rdi
    push rsi
    push rdx
    push rcx
    push r8
    push r9
    push r10
    push r11
    call keyboard_handler_c
    pop r11
    pop r10
    pop r9
    pop r8
    pop rcx
    pop rdx
    pop rsi
    pop rdi
    pop rax
    iretq

isr14:
    push rax
    push rdi
    push rsi
    push rdx
    push rcx
    push r8
    push r9
    push r10
    push r11
    call disk_handler_c
    pop r11
    pop r10
    pop r9
    pop r8
    pop rcx
    pop rdx
    pop rsi
    pop rdi
    pop rax
    iretq

.section .bss
.align 16
kernel_stack_bottom:
    .skip 16384 # 16KB stack
kernel_stack_top:
