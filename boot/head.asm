[SECTION .text]
[BITS 32]
global _start
_start:
    xchg bx, bx
    xchg bx, bx
    jmp $