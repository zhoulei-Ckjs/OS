#include "linux/kernel.h"

/**
 * @brief 时钟中断处理函数
 * @param idt_index
 */
void clock_handler(int idt_index)
{
    printk("0x%x\n", idt_index);
}