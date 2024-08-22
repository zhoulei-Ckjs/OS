[bits 32]
[SECTION .text]

extern printk
extern do_no_page               ; 缺页中断处理函数
extern keymap_handler
extern exception_handler

; 进入中断处理程序之前，CPU会先往栈中压入值，不跨态压入三个值:
;	高地址		eflags
;				cs
;	低地址		eip
; 跨态会压入5个值：
;	高地址		eflags
;				cs
;				eip
;				ss
;	低地址		esp
global interrupt_handler_entry
interrupt_handler_entry:
    push msg
    call printk
    add esp, 4

    iret            ; 拿出 CPU 自动压入的寄存器的值，返回进入中断前的程序

; 缺页中断，0x0e
; cpu 压栈顺序
; ebp   高地址
; esp   低地址     Error Code
; TODO 缺页中断 CPU 会压入哪些值
global page_fault
page_fault:
    xchg bx, bx             ; bochs调试断点
    mov eax, [esp]          ; 拿到错误码，0 表示缺页异常，1 表示由读写权限造成的异常。
    test eax, 1			    ;
                            ; 若缺页异常， eax 最后一位为 0，即(0 & 1 == 0)，则 ZF = 1
                            ; 若读写权限异常， eax 最后一位为 1，即(1 & 1 != 0)，则 ZF = 0
    mov edx, cr2            ; 取引起页面异常的线性地址
    push edx
    push eax

    jne .do_wp_page         ; 若 ZF == 0，则跳转 do_wp_page; 否则继续向下执行
    call do_no_page
    iret            ; 拿出 CPU 自动压入的寄存器的值，返回进入中断前的程序
; 读写异常造成的中断
.do_wp_page:
    push wp_msg
    call printk
    iret


; 键盘中断
; 进入中断处理程序之前，CPU会先往栈中压入值，不跨态压入三个值:
;	高地址		eflags
;				cs
;	低地址		eip
; 跨态会压入5个值：
;	高地址		eflags
;				cs
;				eip
;				ss
;	低地址		esp
global keymap_handler_entry
keymap_handler_entry:
    push 0x21               ; 0x21 表示为键盘中断
    call keymap_handler
    add esp, 4

    iret            ; 拿出 CPU 自动压入的寄存器的值，返回进入中断前的程序

; 进入中断处理程序之前，CPU会先往栈中压入值，不跨态压入三个值（这里不会跨态）:
;	高地址		eflags
;				cs
;	低地址		eip
; 跨态会压入5个值：
;	高地址		eflags
;				cs
;				eip
;				ss
;	低地址		esp
; 定义 INTERRUPT_HANDLER 宏，具备 1 个参数。
%macro INTERRUPT_HANDLER 1
global interrupt_handler_%1
interrupt_handler_%1:
    pushad          ; pushad 是一个 x86 架构汇编指令，用于将所有通用寄存器中的值按照固定顺序依次压入堆栈（栈）中
                    ; EAX
                    ; ECX
                    ; EDX
                    ; EBX
                    ; ESP（当前栈指针）
                    ; EBP（栈基指针）
                    ; ESI
                    ; EDI
    push %1
    call exception_handler
    add esp, 4      ; 平栈，因为 push %1 压入了 1 个参数

    popad           ; 推出所有压入寄存器的值

    iret            ; 中断返回，CPU会先往栈中压入值，不跨态压入三个值，iret就是弹出这三个值
%endmacro

; 对函数的实现
INTERRUPT_HANDLER 0x00; 除零异常
INTERRUPT_HANDLER 0x01; debug
INTERRUPT_HANDLER 0x02; non maskable interrupt
INTERRUPT_HANDLER 0x03; breakpoint

INTERRUPT_HANDLER 0x04; overflow
INTERRUPT_HANDLER 0x05; bound range exceeded
INTERRUPT_HANDLER 0x06; invalid opcode
INTERRUPT_HANDLER 0x07; device not avilable

INTERRUPT_HANDLER 0x08; double fault
INTERRUPT_HANDLER 0x09; coprocessor segment overrun
INTERRUPT_HANDLER 0x0a; invalid TSS
INTERRUPT_HANDLER 0x0b; segment not present

INTERRUPT_HANDLER 0x0c; stack segment fault
INTERRUPT_HANDLER 0x0d; general protection fault
INTERRUPT_HANDLER 0x0e; 缺页中断
INTERRUPT_HANDLER 0x0f; reserved

INTERRUPT_HANDLER 0x10; x87 floating point exception
INTERRUPT_HANDLER 0x11; alignment check
INTERRUPT_HANDLER 0x12; machine check
INTERRUPT_HANDLER 0x13; SIMD Floating - Point Exception

INTERRUPT_HANDLER 0x14; Virtualization Exception
INTERRUPT_HANDLER 0x15; Control Protection Exception

INTERRUPT_HANDLER 0x16; reserved
INTERRUPT_HANDLER 0x17; reserved
INTERRUPT_HANDLER 0x18; reserved
INTERRUPT_HANDLER 0x19; reserved
INTERRUPT_HANDLER 0x1a; reserved
INTERRUPT_HANDLER 0x1b; reserved
INTERRUPT_HANDLER 0x1c; reserved
INTERRUPT_HANDLER 0x1d; reserved
INTERRUPT_HANDLER 0x1e; reserved
INTERRUPT_HANDLER 0x1f; reserved

INTERRUPT_HANDLER 0x20; clock 时钟中断
INTERRUPT_HANDLER 0x21; 键盘中断
INTERRUPT_HANDLER 0x22
INTERRUPT_HANDLER 0x23
INTERRUPT_HANDLER 0x24
INTERRUPT_HANDLER 0x25
INTERRUPT_HANDLER 0x26
INTERRUPT_HANDLER 0x27
INTERRUPT_HANDLER 0x28; rtc 实时时钟
INTERRUPT_HANDLER 0x29
INTERRUPT_HANDLER 0x2a
INTERRUPT_HANDLER 0x2b
INTERRUPT_HANDLER 0x2c
INTERRUPT_HANDLER 0x2d
INTERRUPT_HANDLER 0x2e
INTERRUPT_HANDLER 0x2f

; 定义终端处理函数数组，里面存储了每个中断处理程序的地址
global interrupt_handler_table
interrupt_handler_table:
    dd interrupt_handler_0x00       ; INTERRUPT_HANDLER 0x00 宏替换后就是 interrupt_handler_0x00
    dd interrupt_handler_0x01
    dd interrupt_handler_0x02
    dd interrupt_handler_0x03
    dd interrupt_handler_0x04
    dd interrupt_handler_0x05
    dd interrupt_handler_0x06
    dd interrupt_handler_0x07
    dd interrupt_handler_0x08
    dd interrupt_handler_0x09
    dd interrupt_handler_0x0a
    dd interrupt_handler_0x0b
    dd interrupt_handler_0x0c
    dd interrupt_handler_0x0d
    dd interrupt_handler_0x0e
    dd interrupt_handler_0x0f
    dd interrupt_handler_0x10
    dd interrupt_handler_0x11
    dd interrupt_handler_0x12
    dd interrupt_handler_0x13
    dd interrupt_handler_0x14
    dd interrupt_handler_0x15
    dd interrupt_handler_0x16
    dd interrupt_handler_0x17
    dd interrupt_handler_0x18
    dd interrupt_handler_0x19
    dd interrupt_handler_0x1a
    dd interrupt_handler_0x1b
    dd interrupt_handler_0x1c
    dd interrupt_handler_0x1d
    dd interrupt_handler_0x1e
    dd interrupt_handler_0x1f
    dd interrupt_handler_0x20
    dd interrupt_handler_0x21
    dd interrupt_handler_0x22
    dd interrupt_handler_0x23
    dd interrupt_handler_0x24
    dd interrupt_handler_0x25
    dd interrupt_handler_0x26
    dd interrupt_handler_0x27
    dd interrupt_handler_0x28
    dd interrupt_handler_0x29
    dd interrupt_handler_0x2a
    dd interrupt_handler_0x2b
    dd interrupt_handler_0x2c
    dd interrupt_handler_0x2d
    dd interrupt_handler_0x2e
    dd interrupt_handler_0x2f

msg:
    db "interrupt_handler", 10, 0
wp_msg:
    db "write protected occured", 10, 0