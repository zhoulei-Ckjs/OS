; 0柱面0磁道2扇區
[ORG 0x500]         ;将setup放到0x500处

dw 0x55aa           ; 定义的第一个数，魔数，用于判断是否读盘错误

[SECTION .data]
;----------------
; 构建gdt表用数据
;----------------
MEMORY_BASE equ 0                                               ; 内存开始的位置：基地址
MEMORY_LIMIT equ ((1024 * 1024 * 1024 * 4) / (1024 * 4)) - 1    ; 段界限，20位寻址能力，内存是以4k为一单位划分的
; 选择子，后面进入保护模式就是通过 选择子 访问 描述符 再+ 偏移来访问数据
code_selector equ (1 << 3)                                      ; 代码段选择子，左移3位是最后三位是属性
data_selector equ (2 << 3)                                      ; 数据段选择子

; gdt段，用于构建全局描述符表（global descriptor table）
[SECTION .gdt]
gdt_base:
    dd 0, 0                     ; gdt表的最开始是一个空表，占8字节
gdt_code:
    dw MEMORY_LIMIT & 0xffff                            ; 段界限（15-0）
    dw MEMORY_BASE & 0xffff                             ; 段基址（31-16）
    db (MEMORY_BASE >> 16) & 0xff                       ; 段基址（39-32）
    ;    P_DPL_S_TYPE
    db 0b1_00_1_1010                                    ; 段描述符有效_工作在ring0_非系统段_仅具有执行权限 & 可读
    ;    G_DB_AVL_LIMIT
    db 0b1_1_00_0000 | (MEMORY_LIMIT >> 16 & 0xf)       ; 以4k为单位_32位段_非64位代码段_段界限（最高4位）
    db (MEMORY_BASE >> 24) & 0xff                       ; 段基址，31-24
gdt_data:
    dw MEMORY_LIMIT & 0xffff
    dw MEMORY_BASE & 0xffff
    db (MEMORY_BASE >> 16) & 0xff
    ;    P_DPL_S_TYPE
    db 0b1_00_1_0010                                    ; 段描述符有效_工作在ring0_非系统段_仅具有只读权限
    ;    G_DB_AVL_LIMIT
    db 0b1_1_00_0000 | ((MEMORY_LIMIT >> 16) & 0xf)     ; 以4KB为单位_32位段_非64位代码段_段界限（最高4位）
    db MEMORY_BASE >> 24 & 0xff
; 加载gdt表用gdt_ptr指针
gdt_ptr:
    dw $ - gdt_base - 1         ; 这里是gdtlen - 1，长度-1
    dd gdt_base                 ; GDT基地址

; 代码段
[SECTION .text]
[BITS 16]
global _setup_start
_setup_start:
    ; 由setup进入保护模式，需要构建gdt表，这里先把注释写好
    mov     si, loading
    call    print

    cli                         ; 关闭中断
    ; 打开 A20 线
    in    al,  92h
    or    al,  00000010b
    out   92h, al

    lgdt [gdt_ptr]              ; 加载 gdt

    ; 启动保护模式
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    ; 用跳转来刷新缓存，启用保护模式
    jmp dword code_selector:protect_mode

[bits 32]
protect_mode:
    ; xchg bx, bx; 断点
    mov ax, data_selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax                  ; 初始化段寄存器


    jmp $                       ; 停在这里

; 注意这里要加一个bit 16，因为没进入保护模式前调用32位编码的函数会报错 eip > cs.limit
[BITS 16]
; 如何调用
; mov     si, msg   ; 1 传入字符串
; call    print     ; 2 调用
print:
    mov ah, 0x0e    ; 指定要使用的功能是0x0e，0x0E表示在TTY模式下写字符
    ; bh和bl的设置没有效果
.next:
    mov al, [si]    ; 要输出的字符
    cmp al, 0       ; 字符串不结束就一直打印
    jz .done
    int 0x10        ; 调用bios的10号中断

    inc si
    jmp .next
.done:
    ret

loading:
    db "[setup.asm] : Preparing to enter protect mode ...", 10, 13, 0     ; \n\r