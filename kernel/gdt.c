#include "linux/head.h"

gdt_ptr_t gdt_ptr;              ///< 存储 gdt 信息的全局结构体

void gdt_init()
{
    printk("init gdt...\n");
    printk("%d\n", gdt_ptr.limit);
    printk("%d\n", gdt_ptr.base);

    /// 加载 gdt 表信息，保存到 gdt_ptr 中
    __asm__ volatile ("sgdt gdt_ptr;");
    printk("%d\n", gdt_ptr.limit);
    printk("%d\n", gdt_ptr.base);
}