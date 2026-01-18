section .text
global _start
extern main

_start:
    mov eax, [esp + 4]
    mov ebx, [esp + 8]
    
    push ebx
    push eax
    
    call main
    
    add esp, 8
    ret
