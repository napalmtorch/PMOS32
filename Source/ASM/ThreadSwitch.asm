[GLOBAL _switch_thread]
[EXTERN _update_current_thread]
[EXTERN _current_thread]
[EXTERN _next_thread]

; DS;
; EDI, ESI, EBP, ESP, EBX, EDX, ECX, EAX;
; INT, ERR;
; EIP, CS, EFLAGS, USERESP, SS;

_switch_thread:
    cli
    mov EAX, [_current_thread]

    mov [EAX+4],  EDI
    mov [EAX+8],  ESI
    mov [EAX+12], EBP
    mov [EAX+16], ESP
    mov [EAX+20], EBX
    mov [EAX+24], EDX
    mov [EAX+28], ECX
    mov [EAX+32], EAX

    pushf
    pop ECX
    mov [EAX+52], ECX

    mov EAX, [_next_thread]

    mov ECX, [EAX+52]
    push ECX
    popf

    mov EDI, [EAX+4]
    mov ESI, [EAX+8]
    mov EBP, [EAX+12]
    mov ESP, [EAX+16]
    mov EBX, [EAX+20]
    mov EDX, [EAX+24]
    mov ECX, [EAX+28]

    mov EAX, [_next_thread]
    mov [_current_thread], EAX

    mov EAX, [EAX+32]

    sti
    ret
