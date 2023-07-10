; BIOS程序会将磁盘的第一个扇区的 512 字节加载到 0x7c00 处，为引导程序
[ORG  0x7c00]       ; 告诉汇编器：假定把接下来的机器码假定是加载到内存地址 0x7C00 处执行的。
                    ; 这样下面的代码就是以 0x7c00 为基址的地址开始编写，即第一条指令汇编后的地址为 0x7C00。

; 代码段
[SECTION .text]
[BITS 16]           ; 编码方式
global _start       ; 声明一个全局函数
_start:
    mov sp, 0x8000  ; 设置 boot 的栈顶位置。
    xchg bx, bx     ; bochs 断点。

    ; 当值为3时，这个调用的功能是设置视频模式。具体来说，视频模式3对应于文本模式 25行×80列，16色，8页。
    ; 这通常被认为是典型的DOS文本模式。
    mov ax, 3
    int 0x10        ; 调用 0x10 中断

    ; 加载 setup.asm 代码到 0x500。
    mov si, loading_setup_message
    call print
    call load_setup                     ; 加载 setup 代码到 0x500 位置。
    xchg bx, bx     ; bochs 断点。
    jmp $           ; 停在这里

; 读取 setup.asm 的程序到内存中。
load_setup:
    mov ch, 0       ; 0 柱面。
    mov dh, 0       ; 0 磁头。
    mov cl, 2       ; 2 扇区。
    mov bx, 0x500   ; 数据往哪读。

    mov ah, 0x02    ; 读盘操作。
    mov al, 1       ; 连续读 1 个扇区。
    mov dl, 0x80    ; 驱动器编号，软盘编号 0x00，硬盘编号从 0x80 开始。

    int 0x13
    ret

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

loading_setup_message:
    db "[processing]: loading setup...", 10, 13, 0
        ; 10(0x0a) 对应换行符 '\n'，表示将光标下移一行
        ; 13(0x0d) 对应回车符 '\r'，表示表示将光标移动到当前行的开头。

; 中间都补充 0。
times 510 - ($ - $$) db 0
; 一个扇区要以 0x55aa 结尾，bios 才能识别。
db 0x55, 0xaa