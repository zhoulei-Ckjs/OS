#include "linux/head.h"
#include "string.h"
#include "macro.h"

gdt_ptr_t gdt_ptr;              ///< 存储 gdt 信息的全局结构体

/**
 * @brief 全局描述符表。用于存储段描述符，每个描述符占 8 字符。
 */
gdt_item_t gdt[GDT_SIZE];

void gdt_init()
{
    printk("init gdt...\n");
    /// 加载 gdt 表信息，保存到 gdt_ptr 中
    __asm__ volatile ("sgdt gdt_ptr;");
    memcpy(gdt, gdt_ptr.base, gdt_ptr.limit + 1);   ///< 将 gdt 表的数据拷贝出来

    printk("gdt_item_t size = %d\n", sizeof(gdt_item_t));
    printk("%x\n", gdt[1].limit_low);               ///< 应该是 0xffff
    printk("%x\n", gdt[1].base_low);                ///< 应该是 0x0000
    printk("%x\n", gdt[1].base_low);                ///< 应该是 0x0000
    printk("%x\n", gdt[1].segment);                 ///< 应该是 0x1
    printk("%x\n", gdt[1].DPL);                     ///< 应该是 0x0
    printk("%x\n", gdt[1].present);                 ///< 应该是 0x1
}