; BIOS程序会将磁盘的第一个扇区的 512 字节加载到 0x7c00 处，为引导程序
[ORG  0x7c00]       ; 告诉汇编器：把接下来的机器码假定是加载到内存地址 0x7C00 处执行的。

; 代码段
[SECTION .text]
[BITS 16]           ; 编码方式
global _start       ; 声明一个全局函数
_start:
    xchg bx, bx     ; bochs 断点

    ; 当值为3时，这个调用的功能是设置视频模式。具体来说，视频模式3对应于文本模式80列×25行，16色，8页。
    ; 这通常被认为是典型的DOS文本模式。
    mov ax, 3
    int 0x10        ; 调用 0x10 中断

    mov si, msg     ; 打印字符串
    call print

    jmp $           ; 停在这里

; print函数，用于向屏幕输出字符。
; 调用方式:
;   mov     si, msg   ; 1 传入字符串
;   call    print     ; 2 调用
print:
    mov ah, 0x0e    ; 指定要使用的功能是 0x0e，0x0e 表示在 TTY 模式下写字符。
    mov bh, 0       ; 表示在第 0 个页面输出字符。
    mov bl, 0x01    ; 用于指定颜色，蓝色。
.loop:
    mov al, [si]    ; 要输出的字符。
    cmp al, 0       ; 字符串不结束就一直打印。
    jz .done        ; 字符串结束，跳转到 .done。
    int 0x10        ; 调用bios的10号中断。

    inc si
    jmp .loop
.done:
    ret             ; 结束返回。

; 定义一个 "hello world" 字符串。
msg:
    db "hello, world", 10, 13, 0
        ; 10(0x0a) 对应换行符 '\n'，表示将光标下移一行
        ; 13(0x0d) 对应回车符 '\r'，表示表示将光标移动到当前行的开头。

; 中间都补充 0。
times 510 - ($ - $$) db 0
; 一个扇区要以 0x55aa 结尾，BIOS才能识别。
db 0x55, 0xaa