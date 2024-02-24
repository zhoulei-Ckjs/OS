[bits 32]
[SECTION .text]

extern printk
extern keymap_handler

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

    iret            ; 中断返回
%endmacro

msg:
    db "interrupt_handler", 10, 0