; setup程序所在磁盘的 0 柱面 0 磁道 2 扇区。
; setup 程序加载的内存起始地址。
%define LOAD_SETUP_ADDR 0x7E00

[ORG LOAD_SETUP_ADDR]         ; setup 从 LOAD_SETUP_ADDR 地址开始汇编。

[SECTION .text]
[BITS 16]
global _start
_start:
    mov si, msg
    call print
    jmp $                     ; 停在这里

; print函数，用于向屏幕输出字符。
; 调用方式:
;   mov     si, msg   ; 1 传入字符串
;   call    print     ; 2 调用
print:
    mov ah, 0x0e    ; 指定要使用的功能是 0x0e，0x0e 表示在 TTY 模式下写字符。
    mov bh, 0       ; 表示在第 0 个页面输出字符。
    mov bl, 0x01    ; 蓝色字符，黑色背景。
.loop:
    mov al, [si]    ; 要输出的字符。
    cmp al, 0       ; 字符串不结束就一直打印。
    jz .done        ; 字符串结束，跳转到 .done。
    int 0x10        ; 调用bios的10号中断。

    inc si
    jmp .loop
.done:
    ret             ; 结束返回。

[SECTION .data]
[BITS 16]
msg:
    db "hello, world", 10, 13, 0