[SECTION .text]

; 函数调用会将参数从右向左入栈，所以此时的栈为：
;  高位
;   |         port        +8
;  \ /        返回地址     +4
;  低位       ebp         +0          /// 这一步是 in_byte 内部的 push ebp 的操作
global in_byte
in_byte:
    push ebp;
    mov ebp, esp

    xor eax, eax
    mov edx, [ebp + 8]      ; port
    in al, dx               ; 将端口的数据读到 al 中
    leave                   ; 将栈帧恢复到调用函数前的状态。leave指令会执行以下操作：
                            ;  1.	将ESP寄存器的值设置为EBP寄存器的值
                            ;  2.	将EBP寄存器的值设置为栈顶元素的值
                            ;  3.	弹出栈顶元素
    ret                     ; 从栈中取出返回的内存地址，跳转到这个内存地址继续执行。

global out_byte
; 函数声明在 include/asm/io.h 中
; 函数调用会将参数从右向左入栈，所以此时的栈为(以字节为单位)：
;   value           +8
;   port            +4
;   return addr     +0
out_byte:
    push ebp
    mov ebp, esp
    mov edx, [ebp + 8]          ; port 给 edx
    mov eax, [ebp + 12]         ; value 给 eax
    out dx, al
    leave                       ; mov esp, ebp; pop ebp
    ret                         ; pop ip; jmp ip