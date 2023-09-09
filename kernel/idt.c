#include "linux/traps.h"
#include "string.h"
#include "asm/system.h"
#include "linux/kernel.h"

#define INTERRUPT_TABLE_SIZE    256         ///< 中断门个数
interrupt_descriptor interrupt_table[INTERRUPT_TABLE_SIZE] = {0};       ///< 中断描述符表
xdt_ptr_t idt_ptr;                          ///< 中断向量表重新指向的位置
extern void interrupt_handler();
extern void keymap_handler_entry();         ///< 键盘中断
extern void clock_handler_entry();          ///< 时钟中断
/// 是在汇编 interrupt_handler.asm 中定义的
extern int interrupt_handler_table[0x2f];   ///< 默认中断处理函数

void idt_init()
{
    printk("init idt ...\n");
    for(int i = 0; i < INTERRUPT_TABLE_SIZE; ++i)
    {
        interrupt_descriptor* p = &(interrupt_table[i]);

        memset(p, '\0', sizeof(interrupt_descriptor));

        int handler = (int)interrupt_handler;

        /// 时钟中断
        if(0x20 == i)
        {
            handler = (int)clock_handler_entry;
        }

        /// 键盘中断
        if(0x21 == i)
        {
            handler = (int)keymap_handler_entry;
        }

        /// 0x00 到 0x15 的中断号具有特定的、预先定义好的功能。
        if(i <= 0x15)
        {
            handler = (int)(interrupt_handler_table[i]);    ///< 默认异常处理函数
        }

        p->offset0 = handler & 0xffff;
        p->offset1 = (handler >> 16) & 0xffff;
        p->selector = 1 << 3;               ///< 代码段
        p->reserved = 0;                    ///< 保留不用
        p->type = 0b1110;                   ///< 中断门
        p->segment = 0;                     ///< 系统段
        p->DPL = 0;                         ///< 内核态
        p->present = 1;                     ///< 有效

        /// 最后一个作为结尾标志，验证我们用 lidt 传递的 limit 是否正确，limit 少一 bit 都会导致加载的 idt 少一项。
//        if(i == INTERRUPT_TABLE_SIZE - 1)
//        {
//            memset(p, 0xFF, 8); ///< 我们将数据置为 0b1111...1111 用于测试，可以直观的看到结果
//        }
    }

    write_xdt_ptr(&idt_ptr, (int)interrupt_table, INTERRUPT_TABLE_SIZE * 8 - 1);

    /// volatile 告诉编译器不要优化这段汇编代码，确保汇编指令按书写顺序执行，防止编译器将这段汇编指令移除（即使输出未使用）
    __asm__ volatile("lidt idt_ptr;");      ///< 加载中断描述符表
}