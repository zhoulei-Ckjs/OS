#include "linux/head.h"
#include "string.h"
#include "asm/system.h"
#include "linux/kernel.h"

#define INTERRUPT_TABLE_SIZE    256         ///< 中断门个数
interrupt_descriptor interrupt_table[INTERRUPT_TABLE_SIZE] = {0};       ///< 中断描述符表
char idt_ptr[6] = {0};                      ///< 中断向量表重新指向的位置
extern void interrupt_handler();
extern void keymap_handler_entry();         ///< 键盘中断

void idt_init()
{
    printk("init idt ...\n");
    for(int i = 0; i < INTERRUPT_TABLE_SIZE; ++i)
    {
        interrupt_descriptor* p = &(interrupt_table[i]);

        memset(p, '\0', sizeof(interrupt_descriptor));

        int handler = (int)interrupt_handler;

        /// 键盘中断
        if(0x21 == i)
        {
            handler = (int)keymap_handler_entry;
        }

        p->offset0 = handler & 0xffff;
        p->offset1 = (handler >> 16) & 0xffff;
        p->selector = 1 << 3;               ///< 代码段
        p->reserved = 0;                    ///< 保留不用
        p->type = 0b1110;                   ///< 中断门
        p->segment = 0;                     ///< 系统段
        p->DPL = 0;                         ///< 内核态
        p->present = 1;                     ///< 有效
    }

    *((short*)idt_ptr) = INTERRUPT_TABLE_SIZE * 8;              ///< 每个中断描述符 8 字节，共定义了 256 个。
    *((int*)(idt_ptr + 2)) = (int)(&interrupt_table);           ///< 中断描述符地址。

    BOCHS_DEBUG_MAGIC

    /// volatile 告诉编译器不要优化这段汇编代码，确保汇编指令按书写顺序执行，防止编译器将这段汇编指令移除（即使输出未使用）
    __asm__ volatile("lidt idt_ptr;");      ///< 加载中断描述符表
}