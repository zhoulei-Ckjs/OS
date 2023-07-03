; BIOS程序会将磁盘的第一个扇区的512字节加载到0x7c00处，为引导程序
[ORG  0x7c00]

[SECTION .data]
BOOT_MAIN_ADDR equ 0x500        ; setup.asm保存在这里，是由boot进行跳转的地址

; 代码段
[SECTION .text]
[BITS 16]           ; 编码方式
global _start       ; 声明一个全局函数
_start:
    ; 当值为3时，这个调用的功能是设置视频模式。具体来说，视频模式3对应于文本模式80列×25行，16色，8页。
    ; 这通常被认为是典型的DOS文本模式。
    xchg bx,bx
    mov ax, 3
    int 0x10        ;调用0x10号中断

    ; -------------------------------------------------to do------------------------------------------
    ; 需要将磁盘第二扇区的内容读到0x500处，然后跳转到0x500处继续执行
    ; ------------------------------------------------------------------------------------------------

    jmp $           ; 停在这里，程序结束

; 如何调用
; mov     si, msg   ; 1 传入字符串
; call    print     ; 2 调用
print:
    mov ah, 0x0e    ; 指定要使用的功能是0x0e，0x0E表示在TTY模式下写字符
    mov bh, 0       ; 表示在第0个页面输出字符
    mov bl, 0x01    ; 用于指定颜色，蓝色
.loop:
    mov al, [si]    ; 要输出的字符
    cmp al, 0       ; 字符串不结束就一直打印
    jz .done
    int 0x10        ; 调用bios的10号中断

    inc si
    jmp .loop
.done:
    ret

jmp_to_setup:
    db "jump to setup...", 10, 13, 0

; 一个扇区要以0x55aa结尾，BIOS才能识别
times 510 - ($ - $$) db 0
db 0x55, 0xaa