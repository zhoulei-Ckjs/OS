#include "sys/types.h"
#include "linux/kernel.h"

/**
 * @brief 进入中断前由 cpu 自动压栈的 eflags、cs、eip，以及由 pushad 压栈的所有寄存器和我们自己压入的中断号。
 * @param idt_index 中断号
 * @param edi
 * @param esi
 * @param ebp
 * @param esp
 * @param ebx
 * @param edx
 * @param ecx
 * @param eax
 * @param eip 当前程序执行地址。
 * @param cs 代码段寄存器。
 * @param eflags 状态寄存器。
 */
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