; setup程序所在磁盘的 0 柱面 0 磁道 2 扇区。
; setup 程序加载的内存起始地址。
LOAD_SETUP_ADDR equ 0x7E00

; 构建 GDT 表所用数据
GDT_MEMORY_BASE equ 0           ; 内存开始的位置：段地址
GDT_MEMORY_LIMIT equ 0xFFFFF    ; 段界限，20 位寻址空间的最大值。
                                ; 实际上如果以 4K 为单位，即每 bit 代表 4K，则可以访问 2^32 = 4G 的内存。
                                ; 以什么为单位是 gdt 表中每个表项指定的。
code_selector equ (1 << 3)      ; 代码段选择子，标号为 1，最后 3bit 为属性。
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
    cli                         ; 关闭中断。

    ; 打开 A20 线
    in al, 92h
    or al, 00000010b
    out 92h, al

    lgdt [gdt_ptr]              ; 加载 gdt 表。

    ; 启动保护模式
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    ; 用跳转来刷新缓存，启用保护模式
    jmp dword code_selector:protect_mode

[bits 32]
protect_mode:
    ; 在保护模式下，bios 提供的中断程序就不能用了。
    ; 一个原因是因为实模式下一个中断向量占 4B，而保护模式下一个中断向量占 8B，
    ; 首先位数就不同，所以不能用 print 调用 int 0x10 来输出文本了。
    xchg bx, bx                 ; bochs 断点
    jmp $                       ; 停在这里