[GLOBAL _switch_thread]
[EXTERN _update_current_thread]
[EXTERN _current_thread]
[EXTERN _next_thread]

; ESP, EBP, EDI, ESI;
; EAX, EBX, ECX, EDX;
; EIP, CR3, EFLAGS, PADDING;

_switch_thread:
    mov EAX, [_current_thread]

    mov [EAX+0],  ESP
    mov [EAX+4],  EBP
    mov [EAX+8],  EDI
    mov [EAX+12], ESI
    mov [EAX+16], EAX
    mov [EAX+20], EBX
    mov [EAX+24], ECX
    mov [EAX+28], EDX

    mov ECX, CR3
    mov [EAX+36], ECX

    pushf
    pop ECX
    mov [EAX+40], ECX

    mov EAX, [_next_thread]

    mov ECX, [EAX+40]
    push ECX
    popf

    mov ESP, [EAX+0]
    mov EBP, [EAX+4]
    mov EDI, [EAX+8]
    mov ESI, [EAX+12]
    mov EBX, [EAX+20]
    mov ECX, [EAX+36]
    mov CR3, ECX
    mov ECX, [EAX+24]
    mov EDX, [EAX+28]

    mov [_current_thread], EAX
    mov EAX, [EAX+16]
    
    ret
