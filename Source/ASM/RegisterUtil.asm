[GLOBAL _read_cr0]
[GLOBAL _read_cr2]
[GLOBAL _read_cr3]
[GLOBAL _write_cr0]
[GLOBAL _write_cr2]
[GLOBAL _write_cr3]
[GLOBAL _reload_segments]

_read_cr0:
    mov EAX, CR0
    retn

_read_cr2:
    mov EAX, CR2
    retn

_read_cr3:
    mov EAX, CR3
    retn

_write_cr0:
    push EBP
    mov  EBP, ESP
    mov  EAX, [EBP+4]
    mov  CR0, EAX
    pop  EBP
    ret

_write_cr2:
    push EBP
    mov EBP, ESP
    mov EAX, [EBP+4]
    mov CR2, EAX
    pop EBP
    ret

_write_cr3:
    mov EAX, [ESP+4]
    mov CR3, EAX
    ret

_reload_segments:
    mov AX, 0x10
    mov DS, AX
    mov ES, AX
    mov FS, AX
    mov GS, AX
    mov SS, AX
    jmp 0x08:_reload_segments_end
_reload_segments_end:
    ret