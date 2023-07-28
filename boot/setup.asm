LOAD_SETUP_ADDR equ 0x7E00      ; setup 程序加载的内存起始地址。
LOAD_KERNEL_ADDR equ 0x00       ; 内核加载的位置

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

    ; 打开 A20 线，开启后可以访问 1M 以上的内存。
    ; 在 8086 和 8088 CPU中，只有 20 根地址线，即地址访问为 2^20 = 1M，超过地址后 CPU 的处理是地址回环
    ; 即访问超过 1M 的字节，如访问 0x40 000 001 即访问 0X00 000 001 的地址。
    ; 而 80286 及以后的 CPU 总线个数增加，如 80286 有 24 条地址总线，但是为了保持和前面兼容，超过 1M 地址也会回环
    ; 为了访问 1M 以后的内存，则设计了 A20 地址开关，打开 A20 开关后，就能充分利用 24 根地址总线了
    ; 访问内存的范围也从 1M 变成 1M * 2^4 = 16M，这就是 A20 地址线的由来
    in al, 92h
    or al, 00000010b
    out 92h, al

    lgdt [gdt_ptr]              ; 加载 gdt 表。

    ; 启动保护模式
    mov eax, cr0                ; 读取 CR0 控制寄存器。
    or eax, 1                   ; 设置 PE 位（位0），开启保护模式。
    mov cr0, eax                ; 写回 CR0，正式开启保护模式。

    ; 用跳转来刷新缓存，启用保护模式
    jmp dword code_selector:protect_mode
        ; 使用段选择子进行远跳转，会更新 cs 值，cs = code_selector。
        ; 这句话在实模式下不能使用，实模式用 基址:偏移 进行跳转，而不是 选择子:偏移。

[bits 32]
protect_mode:
    ; 在保护模式下，bios 提供的中断程序就不能用了。
    ; 一个原因是因为实模式下一个中断向量占 4B，而保护模式下一个中断向量占 8B，
    ; 首先位数就不同，所以不能用 print 调用 int 0x10 来输出文本了。

    ; 初始化段寄存器
    ; 进入保护模式后，段寄存器不再使用段地址，而是用 selector 查 GDT，获取段基址、限制、访问权限等。
    ; 如果你不重载这些段寄存器，它们仍然保存着实模式的段地址，会导致访问内存异常或严重错误。
    mov ax, data_selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; 打印 “加载内核”
    mov eax, 1                  ; 文字输出的行。
    mov ebx, 0                  ; 文字输出的列。
    mov esi, loading_kernel     ; 待输出字符串。
    call print

    mov ecx, 2      ; 从哪个扇区开始读，lba 读硬盘方式是以下标 0 开始的
    mov bl, SYSTEM_SECTORS      ; 读取扇区数量
    mov edi, LOAD_KERNEL_ADDR   ; 将磁盘读取到内存位置
    call read_disk  ; 读取磁盘

    ; 打印 “内核加载完成”
    mov eax, 2                  ; 文字输出的行。
    mov ebx, 0                  ; 文字输出的列。
    mov esi, done_loading       ; 待输出字符串。
    call print


    ; 打印 “跳转到内核”
    mov eax, 3
    mov ebx, 0
    xchg bx, bx
    mov esi, jumping_to_kernel
    call print

    jmp dword code_selector:LOAD_KERNEL_ADDR
    ud2             ; 如果跳转失败或者控制流莫名其妙回来了，就触发非法指令异常。

; LBA(Logical Block Addressing) 方式读取磁盘。
; 使用方式：
;    mov ecx, 1                 ; 从哪个扇区开始读，LBA读硬盘方式是以下标 0 开始的。
;    mov bl, 1                  ; 读取扇区数量。
;    mov edi, <your destination>; 待读入的内存位置。
read_disk:
    ; 0x1F2 8bit 指定读取或写入的扇区数
    mov dx, 0x1F2
    mov al, bl                  ; 扇区数
    out dx, al                  ; 向 0x1F2 端口发送要读取的扇区数

    ; 0x1F3 8bit，指定了待读取扇区开始数的 0-7 位。
    mov dx, 0x1F3
    mov eax, ecx
    out dx, al                  ; 拿到待读取扇区的低 8 位。

    ; 0x1F4 8bit，指定了待读取扇区开始数的 8-15 位。
    mov dx, 0x1F4
    shr eax, 8                  ; 右移 8 位。
    out dx, al

    ; 0x1F5 8bit，指定了待读取扇区开始数的 16-23 位。
    mov dx, 0x1F5
    shr eax, 8
    out dx, al

    ; 0x1F6 8bit
    ; bit: |  7  |     6     |  5  |        4         |     3 2 1 0     |
    ;      |  1  |  chs = 0  |  1  |  0: Master Disk  |  24-27 bits of  |
    ;      |     |  lba = 1  |     |  1: Slave Disk   |   lba address   |
    mov dx, 0x1F6
    shr eax, 8
    and al, 0x0F                ; 高 4 位清 0，低 4 位保留地址。
    or al, 0xE0                 ; 高 4 位置为 0b1110。
    out dx, al

    ; 0x1F7 8bit，命令或状态端口
    mov dx, 0x1F7
    mov al, 0x20                ; 发送读盘命令
    out dx, al

.read_check:
    ; 检测硬盘状态
    mov dx, 0x1F7
    in al, dx
    and al, 0x88                ; 取硬盘状态的第 3、7 位
    cmp al, 0x08                ; 硬盘数据准备好了且不忙了
    jnz .read_check             ; 否则就一直检查下去

    ; 从0x1F0 读数据
    mov dx, 0x1F0
    movzx ecx, bl
    shl ecx, 8                  ; 每块盘 512 字节，要循环 256 次，每次读 2 字节

.read_data:
    in ax, dx                   ; 读取 2 字节
    mov [edi], ax               ; 放到 edi 的位置
    add edi, 2
    loop .read_data
    ret

; 调用方式：
;   mov eax, <line you want to print>   ; 文字输出的行。
;   mov ebx, <column you want to print> ; 文字输出的列。
;   mov esi, <msg to be printed>        ; 待输出字符串。
print:
    ; 计算显示位置，每行 80 个字符，一个字符 2 个字节。
    imul eax, 80
    add eax, ebx
    imul eax, 2

    add eax, 0xB8000            ; 显存映射位置。
    mov edi, eax

    cld                         ; 使地址沿增加方向改变。
    mov ah, 0xC                 ; 黑底红字。

.show:
    lodsb                       ; 从 ds:si 中读取 1 个字符到 al 中。
    test al, al                 ; 用于判断 al 是否为 0，若是 0 则会将 ZF 置为 1。
    jz .done                    ; 若 ZF 为 1，跳转到 .done。
    mov [gs:edi], ax            ; 将字符放到视频地址。
    add edi, 2                  ; 增加 2 个字节。
    jmp .show
.done:
    ret

[SECTION .data]
[BITS 32]

loading_kernel:
    db "[setup.asm] : loading kernel ...", 0
done_loading:
    db "[setup.asm] : done loading kernel ", 0
jumping_to_kernel:
    db "[setup.asm] : now, jumping to kernel ...", 0    ; 这里如果少写了","，会导致 jumping_to_kernel 没编译出来