extern printk
extern keymap_handler
extern exception_handler

; 进入中断处理程序之前，CPU会先往栈中压入值，不跨态压入三个值:
;   高地址       eflags
;               cs
;   低地址       eip
; 跨态会压入5个值：
;   高地址       ss
;               esp
;               eflags
;               cs
;   低地址       eip
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

; 进入中断处理程序之前，CPU会先往栈中压入值，不跨态压入三个值:
;   高地址       eflags
;               cs
;   低地址       eip
; 跨态会压入5个值：
;   高地址       ss
;               esp
;               eflags
;               cs
;   低地址       eip
global interrupt_handler
interrupt_handler:
    push msg
    call printk
    add esp, 4

    iret            ; 拿出 CPU 自动压入的寄存器的值，返回进入中断前的程序

; 键盘中断
; 进入中断处理程序之前，CPU会先往栈中压入值，不跨态压入三个值:
;   高地址       eflags
;               cs
;   低地址       eip
; 跨态会压入5个值：
;   高地址       ss
;               esp
;               eflags
;               cs
;   低地址       eip
global keymap_handler_entry
keymap_handler_entry:
    push 0x21       ; 0x21 表示为键盘中断
    call keymap_handler
    add esp, 4

    iret            ; 拿出 CPU 自动压入的寄存器的值，返回进入中断前的程序

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
INTERRUPT_HANDLER 0x0e; page fault
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

msg:
    db "interrupt_handler", 10, 0