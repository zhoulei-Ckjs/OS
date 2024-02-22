#include "../include/linux/kernel.h"
#include "../include/linux/head.h"
#include "../include/string.h"
#include "../include/asm/system.h"

#define INTERRUPT_TABLE_SIZE    256         // 中断门个数

interrupt_gate_t interrupt_table[INTERRUPT_TABLE_SIZE] = {0};
char idt_ptr[6] = {0};                      // 中断向量表重新指向的位置

extern void interrupt_handler_entry();
extern void keymap_handler_entry();         ///< 键盘中断

void idt_init()
{
    printk("init idt...\n");
    for (int i = 0; i < INTERRUPT_TABLE_SIZE; ++i)
    {
        interrupt_gate_t* p = &interrupt_table[i];

        int handler = (int)interrupt_handler_entry;

        /// 键盘中断
        /// 0 - 31 是系统保留终端，32 是时钟， 33 是键盘中断
        if (0x21 == i)
        {
            handler = (int)keymap_handler_entry;
        }

        p->offset0 = handler & 0xffff;
        p->offset1 = (handler >> 16) & 0xffff;
        p->selector = 1 << 3; // 代码段
        p->reserved = 0;      // 保留不用
        p->type = 0b1110;     // 中断门
        p->segment = 0;       // 系统段
        p->DPL = 0;           // 内核态
        p->present = 1;       // 有效
    }

    /// 让CPU知道中断向量表
    *(short*)idt_ptr = INTERRUPT_TABLE_SIZE * 8;        ///< 每个中断描述符 8 字节，共定义了 256 个。
    *(int*)(idt_ptr + 2) = interrupt_table;             ///< 中断描述符地址

    BOCHS_DEBUG_MAGIC
    BOCHS_DEBUG_MAGIC

    asm volatile("lidt idt_ptr;");
}