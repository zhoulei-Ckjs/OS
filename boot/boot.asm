; BIOS程序会将磁盘的第一个扇区的 512 字节加载到 0x7c00 处，为引导程序
[ORG  0x7c00]       ; 告诉汇编器：把接下来的机器码假定是加载到内存地址 0x7C00 处执行的。

; 代码段
[SECTION .text]
[BITS 16]           ; 编码方式
global _start       ; 声明一个全局函数
_start:
    xchg bx, bx
    xchg bx, bx     ; bochs 断点

    jmp $           ; 停在这里

; 中间都补充 0。
times 510 - ($ - $$) db 0
; 一个扇区要以 0x55aa 结尾，BIOS才能识别。
db 0x55, 0xaa