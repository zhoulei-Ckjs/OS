#include "linux/head.h"
#include "string.h"
#include "macro.h"

gdt_ptr_t gdt_ptr;              ///< 存储 gdt 信息的全局结构体

/**
 * @brief 用于存储 gdt 表中每一个段描述符，每个描述符占 8 字符。
 */
u64 gdt[GDT_SIZE];

void gdt_init()
{
    printk("init gdt...\n");
    /// 加载 gdt 表信息，保存到 gdt_ptr 中
    __asm__ volatile ("sgdt gdt_ptr;");
    memcpy(gdt, gdt_ptr.base, gdt_ptr.limit + 1);   ///< 将 gdt 表的数据拷贝出来
    printk("%X\n", (gdt[1]) & 0xffff);          /// 0xffff
    printk("%X\n", (gdt[1] >> 40) & 0xff);
}