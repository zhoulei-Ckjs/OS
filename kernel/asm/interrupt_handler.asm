extern printk

; 进入中断处理程序之前，CPU会先往栈中压入值。
; 不跨态压入三个值:
;   高地址         eflags
;                 cs
;   低地址         eip
; 跨态会压入5个值：
;   高地址         eflags
;                 cs
;                 eip
;                 ss
;   低地址         esp
global interrupt_handler
interrupt_handler:
    push msg
    call printk
    add esp, 4

    iret            ; 拿出 CPU 自动压入的寄存器的值，返回进入中断前的程序

msg:
    db "interrupt_handler", 10, 0