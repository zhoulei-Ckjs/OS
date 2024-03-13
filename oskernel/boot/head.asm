dw 0x55aa
[SECTION .text]
[BITS 32]

extern kernel_main

global _start
_start:
; 配置8259a芯片，响应中断用
.config_8259a:
    ; 向主发送ICW1
    mov al, 11h                 ; 000_1_0_0_0_1，必须为0_必须为1_边沿触发_8字节中断向量_级联8259a_需要发送ICW4
    out 20h, al
    ; 向从发送ICW1
    out 0a0h, al

    ; 向主发送ICW2
    mov al, 20h                 ; 主片中断号从 0x20 开始，即 32，因为 0-31 为系统保留的中断
    out 21h, al

    ; 向从发送ICW2
    mov al, 28h                 ; 继主片中断号从 0x28 开始，因为一个 8259a 芯片占 8 个中断。
    out 0a1h, al

    ; 向主发送ICW3
    mov al, 04h                 ; 00000100，也即 IR2 级联从片
    out 21h, al

    ; 向从发送ICW3
    mov al, 02h                 ; 00000010，即从片 IR1 与主片相连
    out 0A1h , al

    ; 向主发送ICW4
    mov al, 003h                ; 0000 00_1_1，0000 00_自动EOI_X86模式
    out 021h, al

    ; 向从发送ICW4
    out 0A1h, al                ; 0000 00_1_1，0000 00_自动EOI_X86模式

; 屏蔽所有中断
.enable_8259a_main:
    mov al, 11111111b
    out 21h, al

; 屏蔽从芯片所有中断响应
.enable_8259a_slave:
    mov al, 11111111b
    out 0A1h, al

; 调用c程序
.enter_c_world:
    call kernel_main
    jmp $