; 0 柱面 0 磁道 2 扇区
[ORG 0x500]         ; setup 从 0x500 地址开始汇编

[SECTION .data]
[BITS 16]
msg:
    db "hello, world", 10, 13, 0