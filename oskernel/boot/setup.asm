; 0柱面0磁道2扇區
[ORG 0x500]         ;将setup放到0x500处

dw 0x55aa           ; 定义的第一个数，魔数，用于判断是否读盘错误

[SECTION .data]
;----------------------------------------构建gdt表用数据---------------------------------------
MEMORY_BASE equ 0                                               ; 内存开始的位置：基地址
MEMORY_LIMIT equ ((1024 * 1024 * 1024 * 4) / (1024 * 4)) - 1    ; 段界限，20位寻址能力，内存是以4k为一单位划分的
B8000_SEG_BASE equ 0xb8000                                      ; 显存开始位置
B8000_SEG_LIMIT equ 0x7fff                                      ; 显存长度
; 选择子，后面进入保护模式就是通过 选择子 访问 描述符 再+ 偏移来访问数据
CODE_SELECTOR equ (1 << 3)                                      ; 代码段选择子，左移3位是最后三位是属性
DATA_SELECTOR equ (2 << 3)                                      ; 数据段选择子
B8000_SELECTOR equ (3 << 3)                                     ; 显存段选择子
;--------------------------------------------over--------------------------------------------


;------------------------------------------内核用数据------------------------------------------
KERNEL_MAIN_ADDR equ 0x1200    ; kernel保存在这里，是由setup进行跳转的地址
;--------------------------------------------over--------------------------------------------


;-------------------------------------用于存储内存检测的数据-------------------------------------
; 内存检测需要调用 BIOS 中断，进行多次检测（因为可能有多块连续的内存），每次检测我们给 BISO 传递 20 字节，让
; BIOS 将本次检测结果写到这个内存中。
ARDS_TIMES_BUFFER equ 0x1100    ; 用于记录检测了多少次内存，如检测了5次，则共有5块分离的内存
ARDS_BUFFER equ 0x1102          ; 从这个地址开始，填写检测结果，检测 5 次，则从这个地址写 20 * 5 = 100 字节。
ARDS_TIMES dw 0                 ; define word，定义一个字16位，用于内存检测次数计算，检测结果最终要写到 ARDS_TIMES_BUFFER 的地址。

; 存储填充以后的offset，下次检测的结果接着写。
; 也就是上面的写了 20 × 5 = 100 字节后，的结尾在哪里。我们将这个结尾存储到这个值中。
CHECK_BUFFER_OFFSET dw 0
;--------------------------------------------over--------------------------------------------


; gdt段，用于构建全局描述符表（global descriptor table）
[SECTION .gdt]
gdt_base:
    dd 0, 0                     ; gdt表的最开始是一个空表，占8字节
gdt_code:
    dw MEMORY_LIMIT & 0xffff                            ; 段界限（15-0）     ---> 0xffff
    dw MEMORY_BASE & 0xffff                             ; 段基址（31-16）    ---> 0x0000
    db (MEMORY_BASE >> 16) & 0xff                       ; 段基址（39-32）    ---> 0x00
    ;    P_DPL_S_TYPE
    db 0b1_00_1_1010                                    ; 段描述符有效_工作在ring0_非系统段_仅具有执行权限 & 可读     ---> 0x9A
    ;    G_DB_AVL_LIMIT
    db 0b1_1_00_0000 | (MEMORY_LIMIT >> 16 & 0xf)       ; 以4K为单位_32位段_非64位代码段_段界限（最高4位）           ---> 0xf
    db (MEMORY_BASE >> 24) & 0xff                       ; 段基址，31-24      ---> 0x00
gdt_data:
    dw MEMORY_LIMIT & 0xffff
    dw MEMORY_BASE & 0xffff
    db (MEMORY_BASE >> 16) & 0xff
    ;    P_DPL_S_TYPE
    db 0b1_00_1_0010                                    ; 段描述符有效_工作在ring0_非系统段_仅具有只读权限
    ;    G_DB_AVL_LIMIT
    db 0b1_1_00_0000 | ((MEMORY_LIMIT >> 16) & 0xf)     ; 以4KB为单位_32位段_非64位代码段_段界限（最高4位）
    db MEMORY_BASE >> 24 & 0xff
gdt_b8000:
    dw B8000_SEG_LIMIT & 0xffff                         ; 段界限（15-0）     ---> 0x7fff
    dw B8000_SEG_BASE & 0xffff                          ; 段基址（31-16）    ---> 0x8000
    db B8000_SEG_BASE >> 16 & 0xff                      ; 段基址（39-32）    ---> 0x0b
    ;    P_DPL_S_TYPE
    db 0b1_00_1_0010                                    ; 段描述符有效_工作在ring0_非系统段_仅具有执行权限 & 可读     ---> 0x9A
    ;    G_DB_AVL_LIMIT
    db 0b0_1_00_0000 | (B8000_SEG_LIMIT >> 16 & 0xf)    ; 以1B为单位_32位段_非64位代码段_段界限（最高4位）           ---> 0xf
    db B8000_SEG_BASE >> 24 & 0xff                      ; 段基址，31-24      ---> 0x00
; 加载gdt表用gdt_ptr指针
gdt_ptr:
    dw $ - gdt_base - 1         ; 这里是gdtlen - 1，长度-1
    dd gdt_base                 ; GDT基地址

; 代码段
[SECTION .text]
[BITS 16]
global _setup_start
_setup_start:
    ; 清空寄存器
    mov     ax, 0
    mov     ss, ax
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax
    mov     si, ax

    ; 由setup进入保护模式，需要构建gdt表，这里先把注释写好
    mov     si, loading
    call    print

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

    jc memory_check_error   ; 如果出错，打印错误信息

    add di, cx              ; 下次填充的结果存到下个结构体

    inc dword [ARDS_TIMES]  ; 检测次数 + 1

    cmp ebx, 0              ; 在检测的时候，ebx会被bios修改为连续值用于下次检测，ebx不为0就要继续检测
    jne .loop

    mov ax, [ARDS_TIMES]            ; 保存内存检测次数
    mov [ARDS_TIMES_BUFFER], ax

    mov [CHECK_BUFFER_OFFSET], di   ; 保存offset

.memory_check_success:
    mov si, memory_check_success_msg
    call print

; 进入保护模式
enter_protected_mode:
    cli                         ; 关闭中断

    ; 打开 A20 线
    ; 在 8086 和 8088 CPU中，只有 20 根地址线，即地址访问为 2^20 = 1M，超过地址后 CPU 的处理是地址回环
    ; 即访问超过 1M 的字节，如访问 0x40 000 001 即访问 0X00 000 001 的地址。
    ; 而 80286 及以后的 CPU 总线个数增加，如 80286 有 24 条地址总线，但是为了保持和前面兼容，超过 1M 地址也会回环
    ; 为了访问 1M 以后的内存，则设计了 A20 地址开关，打开 A20 开关后，就能充分利用 24 根地址总线了
    ; 访问内存的范围也从 1M 变成 1M * 2^4 = 16M，这就是 A20 地址线的由来
    in    al,  92h
    or    al,  00000010b
    out   92h, al

    lgdt [gdt_ptr]              ; 加载 gdt

    ; 启动保护模式
    mov eax, cr0
    or  eax, 1
    mov cr0, eax

    ; 用跳转来刷新缓存，启用保护模式，跳转后cs寄存器自动刷新了，所以在保护模式下就不用给cs赋值了
    jmp dword CODE_SELECTOR:protected_mode

;16 BIT 编码，放这里
memory_check_error:
    mov     si, memory_check_error_msg
    call    print

    jmp $

[BITS 32]
protected_mode:
    ; 清空寄存器，不然都是之前的数据
    mov ax, DATA_SELECTOR
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax                  ; 初始化段寄存器

    ; 这个地址的设置是参考 BIOS 内存的可用区域设置的
    mov esp, 0x9fbff            ; 修改栈顶

    mov ecx, 3                  ; 起始扇区
    mov bl, 60                  ; 扇区数量
    mov edi, KERNEL_MAIN_ADDR   ; 读取的目标内存
    call read_disk              ; 读取磁盘

    ; 检测 KERNEL_MAIN_ADDR 位置开头是否是0x55aa，因为在 kernel 最开始定义了0x55aa
    cmp word [KERNEL_MAIN_ADDR], 0x55aa
    jnz p_error                 ; 打印加载错误的信息
    jmp dword CODE_SELECTOR:KERNEL_MAIN_ADDR + 2        ; 跳转到 head
    ud2                         ; 表示出错，未定义行为

p_error:
    mov eax, 2                  ; 要打印的位置-行
    mov ebx, 0                  ; 要打印的位置-列
    jmp error

; 打印加载内核错误，在保护模式下打印，就是将字符串写到0xb8000的位置
; 调用方式：
    ;    mov eax, 3                  ; 要打印的位置-行
    ;    mov ebx, 0                  ; 要打印的位置-列
    ;    jmp error
error:
    ; 计算显示位置，每行80个字符，一个字符2个字节
    imul eax, 80
    add eax, ebx
    imul eax, 2

    add eax, 0xb8000        ; 要显示的位置
    mov edi, eax

    xor esi, esi
    mov esi, .error_data    ; 要显示的字符串

    cld                             ; 使得串操作向前
    mov ah, 0ch                     ; 黑底红字

.1:
	lodsb							; 从ds:si中读取1个字符到al中
	test al, al						; 用于判断al是否为0，若是0则会将ZF置为1
	jz .2							; 若ZF为1，跳转到.2
	mov [gs:edi], ax				; 将字符放到视频地址
	add edi, 2						; 增加2个字节
	jmp .1
.2:									; 显示完毕
	jmp $							; 停到当前不动
.error_data:
    db "[setup.asm] : error loading kernel ...", 0
    ; 意味着读取内核到内存出现了错误

; 读取磁盘代码（lba方式读盘），供32位保护模式调用，实模式下不可调用（因为放在了bits 32下了）
; 调用方式：
; mov ecx, XXX      ; 从哪个扇区开始读，lba扇区下标从0开始
; mov bl, XXX       ; 读取扇区数量
; mov edi, XXX      ; 读取的数据要写入到哪里
read_disk:
    ; 0x1f2 8bit 指定读取或写入的扇区数
    mov dx, 0x1f2
    mov al, bl      ; 扇区数
    out dx, al      ; 向0x1f2发送命令

    ; 0x1f3 8bit lba地址的第八位 0-7
    inc dx
    mov al, cl      ; 从哪个扇区开始读，LBA low
    out dx, al

    ; 0x1f4 8bit lba地址的中八位 8-15
    inc dx
    shr ecx, 8
    mov al, cl      ; LBA中8位
    out dx, al

    ; 0x1f5 8bit iba地址的高八位 16-23
    inc dx
    shr ecx, 8      ; 右移8位
    mov al, cl
    out dx, al      ; 发送LBA高8位

    ; 0x1f6 8bit
    ; 0-3 位iba地址的24-27
    ; 4 0表示主盘 1表示从盘
    ; 5、7位固定为1
    ; 6 0表示CHS模式，1表示LAB模式
    inc dx
    shr ecx, 8
    and cl, 0b1111      ; 将高四位置为 0
    mov al, 0b1110_0000 ;
    or al, cl
    out dx, al; 主盘 - LBA 模式

    ; 0x1f7 8bit  命令或状态端口
    inc dx
    mov al, 0x20    ; 发送读盘命令
    out dx, al

    ; 清空循环次数
    xor ecx, ecx
    mov cl, bl          ; 得到读写扇区的数量
.read_sector:
    push cx             ; 保存 cx
    call .read_check    ; 读取一个扇区
    pop cx              ; 恢复 cx
    loop .read_sector
    ret

    ; 检测硬盘状态
    ; 3 0表示硬盘未准备好与主机交换数据 1表示准备好了
    ; 7 0表示硬盘不忙 1表示硬盘忙
    ; 0 0表示前一条指令正常执行 1表示执行出错 出错信息通过0x1f1端口获得
.read_check:
    mov dx, 0x1f7
    in al, dx
    and al, 0b10001000  ; 取硬盘状态的第3、7位
    cmp al, 0b00001000  ; 硬盘数据准备好了且不忙了
    jnz .read_check     ; 否则就一直检查下去

    ; 从0x1f0 读数据
    mov dx, 0x1f0
    mov cx, 256         ; 要循环256次，每次读 2 字节
.read_data:
    in ax, dx           ; 读取 2 字节
    mov [edi], ax       ; 放到edi的位置
    add edi, 2
    loop .read_data
    ret

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
memory_check_error_msg:
    db "memory check fail...", 10, 13, 0
memory_check_success_msg:
    db "memory check success...", 10, 13, 0