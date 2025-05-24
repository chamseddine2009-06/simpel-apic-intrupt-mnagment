
[bits 32]
%macro ISR 1
global isr%1
isr%1:
    cli
    push long 0
    push long %1
    jmp isr_common
%endmacro

%macro ISR_ERR 1
global isr%1
isr%1:
    cli
    ;the erorr code pushed automaticly
    push long %0
    jmp isr_common
%endmacro

%macro IRQ 1
global irq%1
irq%1:
    cli
    ;mov al , 'I'
    ;mov ah , 0xe
    ;mov [0xb8000], ax
    ;mov al , 'R'
    ;mov ah , 0xe
    ;mov [0xb8002], ax
    ;mov al , 'Q'
    ;mov ah , 0xe
    ;mov [0xb8004], ax
    
    push long 0
    push long %1+32
    jmp irq_common
%endmacro



ISR 0
ISR 1
ISR 2
ISR 3
ISR 4
ISR 5
ISR 6
ISR 7
ISR_ERR 8
ISR 9
ISR_ERR 10
ISR_ERR 11
ISR_ERR 12
ISR_ERR 13
ISR_ERR 14
ISR 15
ISR 16
ISR 17
ISR 18
ISR 19
ISR 20
ISR 21
ISR 22
ISR 23
ISR 24
ISR 25
ISR 26
ISR 27
ISR 28
ISR 29
ISR 30
ISR 31
;ISR 32

IRQ 0
IRQ 1
IRQ 2
IRQ 3
IRQ 4
IRQ 5
IRQ 6
IRQ 7
IRQ 8
IRQ 9
IRQ 10
IRQ 11
IRQ 12
IRQ 13
IRQ 14
IRQ 15
;IRQ 16

ISR 128
ISR 177
;global syscall1
;global syscall2
;syscall1:
;    cli
    ;hlt
;    push byte 0
;    push byte 128
;    push eax
;    mov eax, 0xB8000
;    mov byte [eax], 'S'
;    pop eax
;    jmp isr_common;system call

;syscall2:
;    cli
;    hlt
;    push byte 0
;    push byte 177
;    jmp isr_common;system call

extern isr_handler

isr_common:

    pusha
    push ds
    push es
    push fs
    push gs
    mov eax, cr0
    push eax
    mov eax, cr2;becuse some risen , intel and amd never do a regester call cr1 , if you traye to read frome it , you pc will restart
    push eax
    mov eax, cr3
    push eax
    mov eax, cr4
    push eax

    mov ax, 0x10;data segment
    mov ds , ax
    mov es , ax
    mov fs , ax
    mov gs , ax

    mov eax, esp
    push eax
    call isr_handler
    pop eax
    xor eax , eax

    pop eax;cr0
    mov cr4, eax
    pop eax;cr2
    mov cr3, eax
    pop eax;cr3
    mov cr2, eax
    pop eax;cr4
    mov cr0, eax

    pop gs
    pop fs
    pop es
    pop ds

    popa
    add esp, 8 ; clean up our errcode and isrnum.....
    sti
    iret ;pop cs, eip , eflags , ss and esp





extern irq_handler
irq_common:
    pusha
    push ds
    push es
    push fs
    push gs
    mov eax, cr0
    push eax
    mov eax, cr2;becuse some risen , intel and amd never do a regester call cr1 , if you taye to read frome it , you pc will restart
    push eax
    mov eax, cr3
    push eax
    mov eax, cr4
    push eax

    mov ax, 0x10;data segment
    mov ds , ax
    mov es , ax
    mov fs , ax
    mov gs , ax

    mov eax, esp
    push eax
    call irq_handler
    pop eax

    pop eax;cr0
    mov cr4, eax
    pop eax;cr2
    mov cr3, eax
    pop eax;cr3
    mov cr2, eax
    pop eax;cr4
    mov cr0, eax

    pop gs
    pop fs
    pop es
    pop ds

    popa
    add esp, 8 ; clean up our errcode and isrnum.....
    sti
    iret ;pop cs, eip , eflags , ss and esp


