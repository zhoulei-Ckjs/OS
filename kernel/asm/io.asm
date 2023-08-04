[BITS 32]

[SECTION .text]
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