[org 0x7c00]                        
KERNEL_LOCATION equ 0x1000
jmp start

nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop



start:
mov eax, cr0
;or eax, 1
and eax, 0xffffefff
mov cr0, eax

;mov [BOOT_DISK], dl                 

                                    
mov ax, 0
mov ds, ax
mov es, ax
mov fs, ax

; mov bp, 0x8000
;mov sp, bp


;mov dh, 7

mov ah, 0x02
mov al, 40
mov ch, 0x00
mov dh, 0x00
mov cl, 0x02
mov bx, KERNEL_LOCATION
;mov dl, [BOOT_DISK]
int 0x13                ; no error management, do your homework!
jc DERR
                                    
;mov ah, 0x0
;mov al, 0x3
;int 0x10                ; text mode
;;;;;;;;;;;;;Apnded;;;;;;;;;;;;;

mov  di, 0x5000;
xor  ebx, ebx
xor  bp, bp
mov  edx, 0x0534D4150
mov  eax, 0xE801
mov  [es:di+20], dword 1
mov  ecx, 24
int  0x15


mov ax , 0x2401
int 0x15



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
CODE_SEG equ GDT_code - GDT_start
DATA_SEG equ GDT_data - GDT_start

cli
lgdt [GDT_descriptor]
lidt [idt]
nop
nop
nop
nop
nop
nop
nop
mov eax, cr0

or eax, 1
;and eax, 0xffffefff
mov cr0, eax
nop
nop
nop
nop
nop
nop
nop
nop
nop

jmp 0x8:start_protected_mode
nop
nop
nop
nop
nop
nop
nop
nop
nop


jmp $
DERR:
    ;pop ax
    ;mov bp , DERRS
    mov ah , 0x0e
    mov al , 'D'
    int 0x10
    mov al , 'i'
    int 0x10
    mov al , 's'
    int 0x10
    mov al , 'k'
    int 0x10
    mov al , 'E'
    int 0x10
    mov al , 'r'
    int 0x10
    mov al , 'r'
    int 0x10
    hlt



[bits 32]
start_protected_mode:
 mov ax, 0x10
	mov ds, ax	
	mov es, ax
	mov fs, ax
	mov gs ,ax
	mov ss, ax
	mov esp, 0x90000
	mov ebp, esp 
	;hlt
	mov [0x5000],ecx
	mov [0x5002],ebx


	mov al, 'B'
	mov ah, 0x0e; white on black
	mov[0xb8000], ax ;VidioMemory
	;hlt
	jmp 0x8:KERNEL_LOCATION

                                     
BOOT_DISK: db 0

GDT_start:
    GDT_null:
        dd 0x0
        dd 0x0

    GDT_code:
        dw 0xffff
        dw 0x0
        db 0x0
        db 0b10011010
        db 0b11001111
        db 0x0

    GDT_data:
        dw 0xffff
        dw 0x0
        db 0x0
        db 0b10010010
        db 0b11001111
        db 0x0

GDT_end:

GDT_descriptor:
    dw GDT_end - GDT_start - 1
    dd GDT_start
idt:
	dw 0
	dd 0
 
times 510-($-$$) db 0              
dw 0xaa55
