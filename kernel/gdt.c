#include "linux/traps.h"
#include "string.h"
#include "macro.h"
#include "linux/kernel.h"
#include "asm/system.h"

xdt_ptr_t gdt_ptr;              ///< 存储 gdt 信息的全局结构体

/**
 * @brief 全局描述符表。用于存储段描述符，每个描述符占 8 字符。
 */
gdt_item_t gdt[GDT_SIZE];

/**
 * @brief ring3 代码段和数据段选择子
 */
int r3_code_selector;
int r3_data_selector;

/**
 * @brief 创建 ring3 级别代码段描述符
 * @param gdt_index gdt 表中的位置
 * @param base 基地址
 * @param limit 限长
 */
static void r3_gdt_code_item(int gdt_index, int base, int limit)
{
    /// 在实模式时已经构建了 4 个全局描述符，0 是空的，1 是 ring0 的代码段，2 是 ring0 的数据段，3 是 ring0 的显存段
    if(gdt_index < 4)
    {
        printk("the gdt_index:%d has already been used...\n", gdt_index);
        return;
    }

    gdt[gdt_index].limit_low = limit & 0xffff;
    gdt[gdt_index].base_low = base & 0xffffff;
    gdt[gdt_index].type = 0b1000;                            ///< 仅具有执行权限
    gdt[gdt_index].segment = 1;                              ///< 1 表示代码段或数据段，0 表示系统段
    gdt[gdt_index].DPL = 0b11;                               ///< CPU 工作在 ring3 级别
    gdt[gdt_index].present = 1;                              ///< 当前段在内存中
    gdt[gdt_index].limit_high = limit >> 16 & 0xf;
    gdt[gdt_index].available = 0;
    gdt[gdt_index].long_mode = 0;                            ///< 非64位代码段
    gdt[gdt_index].big = 1;                                  ///< 32位的段
    gdt[gdt_index].granularity = 1;                          ///< 4K 为单位
    gdt[gdt_index].base_high = base >> 24 & 0xff;
}

/**
 * @brief 创建 ring3 级别数据段描述符
 * @param gdt_index gdt 表中的位置
 * @param base 基地址
 * @param limit 限长
 */
static void r3_gdt_data_item(int gdt_index, int base, int limit)
{
    /// 在实模式时已经构建了 4 个全局描述符，0 是空的，1 是 ring0 的代码段，2 是 ring0 的数据段，3 是 ring0 的显存段
    if(gdt_index < 4)
    {
        printk("the gdt_index:%d has already been used...\n", gdt_index);
        return;
    }

    gdt[gdt_index].limit_low = limit & 0xffff;
    gdt[gdt_index].base_low = base & 0xffffff;
    gdt[gdt_index].type = 0b0010;                            ///< 具有读写权限
    gdt[gdt_index].segment = 1;                              ///< 1 表示代码段或数据段，0 表示系统段
    gdt[gdt_index].DPL = 0b11;                               ///< CPU 工作在 ring3 级别
    gdt[gdt_index].present = 1;                              ///< 当前段在内存中
    gdt[gdt_index].limit_high = limit >> 16 & 0xf;
    gdt[gdt_index].available = 0;
    gdt[gdt_index].long_mode = 0;                            ///< 非 64 位代码段
    gdt[gdt_index].big = 1;                                  ///< 32 位的段
    gdt[gdt_index].granularity = 1;                          ///< 4K 为单位
    gdt[gdt_index].base_high = base >> 24 & 0xff;
}

void gdt_init()
{
    printk("init gdt...\n");
    /// 加载 gdt 表信息，保存到 gdt_ptr 中
    __asm__ volatile ("sgdt gdt_ptr;");
    memcpy(gdt, (void*)(gdt_ptr.base), gdt_ptr.limit + 1);   ///< 将 gdt 表的数据拷贝出来

    /// ring3 级别的描述符
    r3_gdt_code_item(4, 0, 0xfffff);                         ///< 创建代码段。段界限以 4K 为单位，即界限 = 0xfffff * 4K = 4G
    r3_gdt_data_item(5, 0, 0xfffff);                         ///< 创建数据段。

    /// ring3 级别的选择子
    r3_code_selector = 4 << 3 | 0b011;                       ///< 索引为 4，请求特权级为 ring3
    r3_data_selector = 5 << 3 | 0b011;                       ///< 索引为 5，请求特权级为 ring3

    write_xdt_ptr(&gdt_ptr, (int)gdt, sizeof(gdt) - 1);

    __asm__ volatile ("lgdt gdt_ptr;");                      ///< 重新加载 gdt 表
}