global keyboard_interrupt_handler
extern keyboard_handler

keyboard_interrupt_handler:
    pusha
    call keyboard_handler
    mov al, 0x20
    out 0x20, al
    popa
    iret

global timer_interrupt_handler
extern timer_handler

timer_interrupt_handler:
    pusha
    call timer_handler
    mov al, 0x20
    out 0x20, al
    popa
    iret

global syscall_interrupt_handler
extern syscall_dispatch

syscall_interrupt_handler:
    push edx
    push ecx
    push ebx
    push eax
    call syscall_dispatch
    add esp, 16
    iret

