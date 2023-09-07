#include "sys/types.h"
#include "linux/kernel.h"

/**
 * @brief 异常描述信息
 */
char *messages[] =
{
    "#DE Divide Error\0",                                   ///< 除零异常
    "#DB RESERVED\0",
    "--  NMI Interrupt\0",
    "#BP Breakpoint\0",
    "#OF Overflow\0",
    "#BR BOUND Range Exceeded\0",
    "#UD Invalid Opcode (Undefined Opcode)\0",
    "#NM Device Not Available (No Math Coprocessor)\0",
    "#DF Double Fault\0",
    "    Coprocessor Segment Overrun (reserved)\0",
    "#TS Invalid TSS\0",
    "#NP Segment Not Present\0",
    "#SS Stack-Segment Fault\0",
    "#GP General Protection\0",
    "#PF Page Fault\0",
    "--  (Intel reserved. Do not use.)\0",
    "#MF x87 FPU Floating-Point Error (Math Fault)\0",
    "#AC Alignment Check\0",
    "#MC Machine Check\0",
    "#XF SIMD Floating-Point Exception\0",
    "#VE Virtualization Exception\0",
    "#CP Control Protection Exception\0",
};

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
    printk("EXCEPTION : %s \n", messages[idt_index]);
    printk("   VECTOR : 0x%02X\n", idt_index);
    printk("   EFLAGS : 0x%08X\n", eflags);
    printk("       CS : 0x%02X\n", cs);
    printk("      EIP : 0x%08X\n", eip);
    printk("      ESP : 0x%08X\n", esp);
    printk("==========\n");
    while (true);
}