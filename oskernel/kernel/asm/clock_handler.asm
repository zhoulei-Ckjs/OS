[bits 32]
[SECTION .text]

; 没有任务时的时钟中断处理函数，
; void clock_handler(int idt_index)
; 在clock.c中
extern clock_handler
; 当前任务
; 定义在sched.c中
extern current

extern Init_task

; 由clock_handler_entry保存当前进程的信息，由switch_task取出下一个进程的eip和环境继续执行
; 时钟中断入口函数，每次时钟中断都要保存当前任务执行到哪里了，以及寄存器指针
; ecx
; eip
; cs
; eflags
global clock_handler_entry
clock_handler_entry:

    push ecx

    mov ecx, [current]
    cmp ecx, 0
    je .call_handler

    ; 保存当前任务执行到哪里了，寄存器的数值也保存起来
    mov [ecx + 10 * 4], eax
    mov [ecx + 12 * 4], edx
    mov [ecx + 13 * 4], ebx
    mov [ecx + 15 * 4], ebp
    mov [ecx + 16 * 4], esi
    mov [ecx + 17 * 4], edi

    mov eax, [esp + 4]          ; eip 由于栈顶地址在进行函数调用时通常用来保存返回地址，
                                ; 而返回地址就是下一条要执行的指令的地址，所以在函数中可以通过ESP寄存器加上一个偏移量来访问返回地址
    mov [ecx + 8 * 4], eax      ; tss.eip

    mov eax, esp
    add eax, 0x10               ; 这里是找到了最初的esp
    mov [ecx + 4], eax          ; tss.esp0

    mov eax, ecx
    pop ecx
    mov [eax + 11 * 4], ecx

.call_handler:
    push 0x20
    call clock_handler
    add esp, 4

    iret

msg:
    db "clock_handler", 10, 0