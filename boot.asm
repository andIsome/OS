[org 0x7c00]                        
KERNEL_LOCATION equ 0x1000

LOADED_KERNEL: db 0

boot_main:
mov byte [BOOT_DISK], dl

                                    
xor ax, ax                          
mov es, ax
mov ds, ax
mov bp, 0x8000
mov sp, bp

; Set the display mode to text mode
;mov ah, 0x0
;mov al, 0x3
;int 0x10
mov ah, 0x0
mov al, 0x13
int 0x10

; Read data from disk
mov ah, 2  ; Select int type (read)

mov al, 30 ; number of segments to read (MUST BE SMALLER THAN 128)
mov bx, KERNEL_LOCATION ; write to
mov ch, 0  ; cylinder 0 ?
mov cl, 2  ; start at sector 2
mov dh, 0  ; head 0 ?
mov dl, [BOOT_DISK]
int 0x13

jc NODRIVEERROR

mov byte [LOADED_KERNEL], 1 ; unable to read kernel from disk: set err code

NODRIVEERROR:


CODE_SEG equ GDT_code - GDT_start
DATA_SEG equ GDT_data - GDT_start

cli
lgdt [GDT_descriptor]
mov eax, cr0
or eax, 1
mov cr0, eax
jmp CODE_SEG:start_protected_mode

jmp $

global BOOT_DISK
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


[bits 32]
start_protected_mode:
    mov ax, DATA_SEG
	mov ds, ax
	mov ss, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	
	mov ebp, 0x90000		; 32 bit stack base pointer
	mov esp, ebp

    ;mov ax, CODE_SEG + 48 ; Print the CODE_SEG value to debug port 0xE9
    ;out 0xE9, al
    ;mov ax, 10
    ;out 0xE9, al

    mov al, byte [LOADED_KERNEL]
    test al, al
    jnz KERNEL_LOCATION

    cli
    hlt


global CODE_SEGMENT
CODE_SEGMENT: dw CODE_SEG

global DATA_SEGMENT
DATA_SEGMENT: dw DATA_SEG
 
times 510-($-$$) db 0              
dw 0xaa55