#include "../include/linux/types.h"

void exception_handler(int idt_index, int edi, int esi, int ebp, int esp,
                       int ebx, int edx, int ecx, int eax, int eip, char cs, int eflags)
{
    printk("\n==========\n");
    printk("EXCEPTION : %d \n", idt_index);
    printk("   VECTOR : 0x%02X\n", idt_index);
    printk("   EFLAGS : 0x%08X\n", eflags);
    printk("       CS : 0x%02X\n", cs);
    printk("      EIP : 0x%08X\n", eip);
    printk("      ESP : 0x%08X\n", esp);
    printk("==========\n");
    while (true);
}