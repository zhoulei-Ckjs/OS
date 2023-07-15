; setup程序所在磁盘的 0 柱面 0 磁道 2 扇区。
; setup 程序加载的内存起始地址。
LOAD_SETUP_ADDR equ 0x7E00

; 构建 GDT 表所用数据
GDT_MEMORY_BASE equ 0           ; 内存开始的位置：段地址
GDT_MEMORY_LIMIT equ 0xFFFFF    ; 段界限，20 位寻址空间的最大值。
                                ; 实际上如果以 4K 为单位，即每 bit 代表 4K，则可以访问 2^32 = 4G 的内存。
                                ; 以什么为单位是 gdt 表中每个表项指定的。
code_selector equ (1 << 3)      ; 代码段选择子，左移 3 位是最后三位是属性。
                                ; 选择子，进入保护模式就是通过选择子访问地址的，cs:ip 的访问时 cs 指定的选择子，
                                ; 通过选择子获取基址，然后加上 ip 即访问地址。
data_selector equ (2 << 3)      ; 数据段选择子

[ORG LOAD_SETUP_ADDR]           ; setup 从 LOAD_SETUP_ADDR 地址开始汇编。

; gdt 段，用于构建全局描述符表（global descriptor table）
[SECTION .gdt]
[BITS 16]
gdt_base:
    dd 0, 0                     ; gdt表的最开始是一个空表，占8字节
gdt_code:
    dw GDT_MEMORY_LIMIT & 0xffff                        ; 段界限（15-0）
    dw GDT_MEMORY_BASE & 0xffff                         ; 段基址（31-16）
    db (GDT_MEMORY_BASE >> 16) & 0xff                   ; 段基址（39-32）
    ;    P_DPL_S_TYPE
    db 0b1_00_1_1010                                    ; 段描述符有效_工作在ring0_非系统段_仅具有执行权限 & 可读
    ;    G_DB_AVL_LIMIT
    db 0b1_1_00_0000 | (GDT_MEMORY_LIMIT >> 16 & 0xf)   ; 以4k为单位_32位段_非64位代码段_段界限（最高 4 位）
    db (GDT_MEMORY_BASE >> 24) & 0xff                   ; 段基址，31-24
gdt_data:
    dw GDT_MEMORY_LIMIT & 0xffff
    dw GDT_MEMORY_BASE & 0xffff
    db (GDT_MEMORY_BASE >> 16) & 0xff
    ;    P_DPL_S_TYPE
    db 0b1_00_1_0010                                    ; 段描述符有效_工作在ring0_非系统段_仅具有只读权限
    ;    G_DB_AVL_LIMIT
    db 0b1_1_00_0000 | ((GDT_MEMORY_LIMIT >> 16) & 0xf) ; 以4KB为单位_32位段_非64位代码段_段界限（最高4位）
    db GDT_MEMORY_BASE >> 24 & 0xff
gdt_ptr:                        ; 加载gdt表用 gdt_ptr 指针
    dw $ - gdt_base - 1         ; 这里是 gdtlen - 1，长度 - 1
    dd gdt_base                 ; GDT基地址

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