#include "linux/kernel.h"
#include "asm/io.h"

#define PIT_CTRL_REG 0X43       ///< 用于编程 8254 芯片（可编程间隔定时器）的端口，时钟输入频率通常是 1.193182 MHz
#define PIT_CHAN0_REG 0X40      ///< 0x40 端口通常与计数器 0 相关联，用于设置定时器的计时间隔或产生脉冲信号

#define HZ 100                              ///< 每秒产生 100 个时钟中断
#define OSCILLATOR 1193182                  ///< 即每秒钟产生 1193182 个脉冲，这个是 8254 芯片固定的，不能改
#define CLOCK_COUNTER (OSCILLATOR / HZ)     ///< tick 多少次后发生一次中断

void clock_init()
{
    /// 0b00110100 指定了一种特定的工作模式，Rate Generator mode，通常用于产生固定频率的脉冲信号。
    out_byte(PIT_CTRL_REG, 0b00110100);

    /// 设置 tick 多少次后发生一次中断
    /// 由于只能发送两个字节，最大为 0xFFFF，即 65535 次，故中断间的最大间隔为 1193182 / 65535 = 18
    /// 即最慢能每秒 18 个中断。
    out_byte(PIT_CHAN0_REG, CLOCK_COUNTER & 0xff);                  ///< 发送低位
    out_byte(PIT_CHAN0_REG, (CLOCK_COUNTER >> 8) & 0xff);           ///< 发送高位
}

/**
 * @brief 时钟中断处理函数
 * @param idt_index
 */
void clock_handler(int idt_index)
{
    printk("0x%x\n", idt_index);
}