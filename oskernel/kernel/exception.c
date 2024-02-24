#include "../include/linux/types.h"

void exception_handler(int idt_index, int edi, int esi, int ebp, int esp,
                       int ebx, int edx, int ecx, int eax, int eip, char cs, int eflags)
{
    printk("\n==========\n");
    while (true);
}