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

