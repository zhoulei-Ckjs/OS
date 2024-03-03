#include "../../include/linux/kernel.h"
#include "../../include/linux/traps.h"

/**
 * @brief 时钟中断初始化，控制时钟中断频率
 */
void clock_init()
{

}

void clock_handler(int idt_index)
{
    send_eoi(idt_index);
    printk("0x%x\n", idt_index);
}
