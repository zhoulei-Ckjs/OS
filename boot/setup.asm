LOAD_SETUP_ADDR equ 0x7E00      ; setup 程序加载的内存起始地址。

; 1.构建 GDT 表所用数据
GDT_MEMORY_BASE equ 0           ; 内存开始的位置：段地址
GDT_MEMORY_LIMIT equ 0xFFFFF    ; 段界限，20 位寻址空间的最大值。
                                ; 实际上如果以 4K 为单位，即每 bit 代表 4K，则可以访问 2^32 = 4G 的内存。
                                ; 以什么为单位是 gdt 表中每个表项指定的。
B8000_SEG_BASE equ 0xB8000      ; 显存开始位置
B8000_SEG_LIMIT equ 0x7FFF      ; 显存长度
code_selector equ (1 << 3)      ; 代码段选择子，标号为 1，最后 3bit 为属性。
                                ; 选择子，进入保护模式就是通过选择子访问地址的，cs:ip 的访问时 cs 指定的选择子，
                                ; 通过选择子获取基址，然后加上 ip 即访问地址。
data_selector equ (2 << 3)      ; 数据段选择子
b8000_selector equ (3 << 3)     ; 视频段选择子

; 2.内存检测所用数据
; 内存检测需要调用 BIOS 中断，进行多次检测（因为可能有多块连续的内存），
; 每次检测我们给 BISO 传递 20 字节，让 BIOS 将本次检测结果写到这个内存中。
ARDS_TIMES_BUFFER equ 0x8200    ; 用于记录检测了多少次内存，如检测了 5 次，则共有 5 块分离的内存
ARDS_BUFFER equ 0x8202          ; 从这个地址开始，填写检测结果，检测 5 次，则从这个地址写 20 * 5 = 100 字节。

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
    db 0b1_00_1_1010                                    ; 段描述符有效_工作在ring0_非系统段_仅具有执行权限 & 可读的代码段
    ;    G_DB_AVL_LIMIT
    db 0b1_1_00_0000 | (GDT_MEMORY_LIMIT >> 16 & 0xf)   ; 以4k为单位_32位段_非64位代码段_段界限（最高 4 位）
    db (GDT_MEMORY_BASE >> 24) & 0xff                   ; 段基址，31-24
gdt_data:
    dw GDT_MEMORY_LIMIT & 0xffff
    dw GDT_MEMORY_BASE & 0xffff
    db (GDT_MEMORY_BASE >> 16) & 0xff
    ;    P_DPL_S_TYPE
    db 0b1_00_1_0010                                    ; 段描述符有效_工作在ring0_非系统段_具有读/写权限的数据段
    ;    G_DB_AVL_LIMIT
    db 0b1_1_00_0000 | ((GDT_MEMORY_LIMIT >> 16) & 0xf) ; 以4KB为单位_32位段_非64位代码段_段界限（最高4位）
    db GDT_MEMORY_BASE >> 24 & 0xff
gdt_b8000:
    dw B8000_SEG_LIMIT & 0xffff                         ; 段界限（15-0）
    dw B8000_SEG_BASE & 0xffff                          ; 段基址（31-16）
    db B8000_SEG_BASE >> 16 & 0xff                      ; 段基址（39-32）
    ;    P_DPL_S_TYPE
    db 0b1_00_1_0010                                    ; 段描述符有效_工作在ring0_非系统段_具有读/写权限的数据段
    ;    G_DB_AVL_LIMIT
    db 0b0_1_00_0000 | (B8000_SEG_LIMIT >> 16 & 0xf)    ; 以1B为单位_32位段_非64位代码段_段界限（最高4位）
    db B8000_SEG_BASE >> 24 & 0xff                      ; 段基址，31-24
gdt_ptr:                        ; 加载gdt表用 gdt_ptr 指针
    dw $ - gdt_base - 1         ; 这里是 gdtlen - 1，长度 - 1
    dd gdt_base                 ; GDT基地址

[SECTION .text]
[BITS 16]

; 内存检测
memory_check:
    xor ebx, ebx            ; ebx = 0
    mov di, ARDS_BUFFER     ; es:di 指向一块内存   es因为前面已设置为0，这里不重复赋值
.loop:
    mov eax, 0xe820         ; ax = 0xe820
        ; 将值 0xe820 存储在 eax 寄存器中，这通常用作功能码，指示 BIOS 或系统服务程序执行获取系统内存分布信息的功能。
    mov ecx, 20
        ; 将 ECX 寄存器的值设置为 20，这是传递给 BIOS 的缓冲区大小，指明了内存映射记录的大小。
        ; 通常，这个大小为 20 字节（0x14 字节），每个内存映射条目的格式。
    mov edx, 0x534D4150     ; edx = 0x534D4150
        ; 将 EDX 寄存器的值设置为 'SMAP' 的 ASCII 码（0x534D4150）。
        ; 这是一个签名，操作系统通过这个签名来验证 BIOS 返回的信息是否有效。
        ; 这是一个魔术数，用来确认 BIOS 支持这项功能和正确地响应请求。
    int 0x15
        ; 执行中断 15H 调用。它传递了前面设置的参数（EAX=0xE820, ECX=20, EDX='SMAP'），请求 BIOS 返回系统的内存映射。
        ; 这个中断调用会填充先前准备好的内存区域（通过 ES:DI 指向的缓冲区）来返回内存映射信息，
        ; 同时更新 EAX 寄存器为 'SMAP'，以确认操作成功，
        ; 并且将下一次调用所需的 continuation value 返回在 EBX 中，用于连续获取所有内存区域信息。

    jc .memory_check_error   ; 如果出错，打印错误信息

    add di, cx              ; 下次填充的结果存到下个结构体

    inc dword [ARDS_TIMES]  ; 检测次数 + 1

    cmp ebx, 0              ; 在检测的时候，ebx 会被 bios 修改为连续值用于下次检测，ebx 不为 0 就要继续检测
    jne .loop

    mov ax, [ARDS_TIMES]            ; 保存内存检测次数
    mov [ARDS_TIMES_BUFFER], ax

    mov [CHECK_BUFFER_OFFSET], di   ; 保存 offset

.memory_check_success:
    mov si, memory_check_success_msg
    call print_16
    jmp _start

.memory_check_error:
    mov si, memory_check_error_msg
    call print_16
    jmp $

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
    mov eax, 2                  ; 文字输出的行。
    mov ebx, 0                  ; 文字输出的列。
    mov esi, loading_kernel     ; 待输出字符串。
    call print_32
    mov ecx, SYSTEM_START_SECTOR    ; 从哪个扇区开始读，lba 读硬盘方式是以下标 0 开始的
    mov bl, SYSTEM_SECTORS      ; 读取扇区数量
    mov edi, LOAD_KERNEL_ADDR   ; 将磁盘读取到内存位置
    call read_disk  ; 读取磁盘

    ; 打印 “内核加载完成”
    mov eax, 3                  ; 文字输出的行。
    mov ebx, 0                  ; 文字输出的列。
    mov esi, done_loading       ; 待输出字符串。
    call print_32

    ; 打印 “跳转到内核”
    mov eax, 4
    mov ebx, 0
    mov esi, jumping_to_kernel
    call print_32

    jmp dword code_selector:LOAD_KERNEL_ADDR    ; 跳转到内核开始执行。
    ud2             ; 如果跳转失败或者控制流莫名其妙回来了，就触发非法指令异常。

[bits 16]
; print函数，用于向屏幕输出字符。
; 调用方式:
;   mov     si, msg   ; 1 传入字符串
;   call    print_16  ; 2 调用
print_16:
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

[bits 32]
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

    movzx ecx, bl
    shl ecx, 8                  ; 每块盘 512 字节，要循环 256 次，每次读 2 字节

.read_check:
    ; 检测硬盘状态
    mov dx, 0x1F7
    in al, dx
    and al, 0x88                ; 取硬盘状态的第 3、7 位
    cmp al, 0x08                ; 硬盘数据准备好了且不忙了
    jnz .read_check             ; 否则就一直检查下去

    ; 从0x1F0 读数据
    mov dx, 0x1F0

.read_data:
    in ax, dx                   ; 读取 2 字节
    mov [edi], ax               ; 放到 edi 的位置
    add edi, 2
    loop .read_check            ; 取 2 字节后进行检查
    ret

; 调用方式：
;   mov eax, <line you want to print>   ; 文字输出的行。
;   mov ebx, <column you want to print> ; 文字输出的列。
;   mov esi, <msg to be printed>        ; 待输出字符串。
print_32:
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
; 数据段无需指定汇编位数，.data 定义的数据长度不变，如 dw 就是定义了 16bit 的数据。

; 2.内存检测用
ARDS_TIMES dw 0     ; define word，定义一个字16位，用于内存检测次数计算，检测结果最终要写到 ARDS_TIMES_BUFFER 的地址。
; 存储填充以后的 offset，下次检测的结果接着写。
; 也就是上面的写了 20 × 5 = 100 字节后，的结尾在哪里。我们将这个结尾存储到这个值中。
CHECK_BUFFER_OFFSET dw 0

loading_kernel:
    db "[setup.asm] : loading kernel ...", 0
done_loading:
    db "[setup.asm] : done loading kernel ", 0
jumping_to_kernel:
    db "[setup.asm] : now, jumping to kernel ...", 0    ; 这里如果少写了","，会导致 jumping_to_kernel 没编译出来
memory_check_error_msg:
    db "[setup.asm] : memory check fail...", 10, 13, 0
memory_check_success_msg:
    db "[setup.asm] : memory check success...", 10, 13, 0