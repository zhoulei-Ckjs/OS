; setup程序所在磁盘的 0 柱面 0 磁道 2 扇区。
; setup 程序加载的内存起始地址。
%define LOAD_SETUP_ADDR 0x7E00

[ORG LOAD_SETUP_ADDR]         ; setup 从 LOAD_SETUP_ADDR 地址开始汇编。

[SECTION .data]
[BITS 16]
msg:
    db "hello, world", 10, 13, 0