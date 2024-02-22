#include "../include/linux/kernel.h"
#include "../include/linux/head.h"
#include "../include/linux/types.h"
#include "../include/asm/system.h"

#define GDT_SIZE    256

/**
 * @brief 定义一个存储 gdt 信息的全局结构体
 */
xdt_ptr_t gdt_ptr;

/**
 * @brief 用于存储 gdt 表中每一个段描述符，每个描述符占 8 字符
 */
u64 gdt[GDT_SIZE] = {0};

/**
 * @brief r3 代码段和数据段选择子
 */
int r3_code_selector;
int r3_data_selector;

/**
 * @brief 创建 ring3 级别的代码段描述符
 * @param gdt_index 在 gdt 表中的位置
 * @param base 基地址
 * @param limit 限长
 */
static void r3_gdt_code_item(int gdt_index, int base, int limit)
{
    // 在实模式时已经构建了4个全局描述符，0是空的，1是ring0的代码段，2是ring0的数据段，3是ring0的显存段
    // 所以从4开始
    if (gdt_index < 4)
    {
        printk("the gdt_index:%d has already been used...\n", gdt_index);
        return;
    }

    gdt_item_t* item = &gdt[gdt_index];

    item->limit_low = limit & 0xffff;
    item->base_low = base & 0xffffff;
    item->type = 0b1000;                    // 仅具有执行权限
    item->segment = 1;                      // 表示是代码段或者数据段，但是非系统段
    item->DPL = 0b11;                       // CPU 工作在 ring3 级别
    item->present = 1;                      // 当前段在内存中
    item->limit_high = limit >> 16 & 0xf;
    item->available = 0;
    item->long_mode = 0;                    // 非64位代码段
    item->big = 1;                          // 32位的段
    item->granularity = 1;                  // 4K 为单位
    item->base_high = base >> 24 & 0xff;
}

/**
 * @brief 创建 r3 数据段
 * @param gdt_index 在 gdt 表中的位置
 * @param base 基地址
 * @param limit 限长
 */
static void r3_gdt_data_item(int gdt_index, int base, int limit)
{
    // 在实模式时已经构建了4个全局描述符，0是空的，1是ring0的代码段，2是ring0的数据段，3是ring0的显存段
    // 所以从4开始
    if (gdt_index < 4)
    {
        printk("the gdt_index:%d has already been used...\n", gdt_index);
        return;
    }

    gdt_item_t* item = &gdt[gdt_index];

    item->limit_low = limit & 0xffff;
    item->base_low = base & 0xffffff;
    item->type = 0b0010;                        // 具有读写权限
    item->segment = 1;                          // 表示是代码段或者数据段，但是非系统段
    item->DPL = 0b11;                           // CPU 工作在 ring3 级别
    item->present = 1;                          // 当前段在内存中
    item->limit_high = limit >> 16 & 0xf;
    item->available = 0;
    item->long_mode = 0;                        // 非64位代码段
    item->big = 1;                              // 32位的段
    item->granularity = 1;                      // 4K 为单位
    item->base_high = base >> 24 & 0xff;
}

void gdt_init()
{
    printk("init gdt...\n");
    __asm__ volatile ("sgdt gdt_ptr;");             // 加载 gdt 表信息，保存到 gdt_ptr 中
    memcpy(&gdt, gdt_ptr.base, gdt_ptr.limit);      // 拷贝 gdt 中段描述符出来

    // 创建r3用的段描述符：代码段、数据段
    r3_gdt_code_item(4, 0, 0xfffff);        // r3 代码段
    r3_gdt_data_item(5, 0, 0xfffff);

    // 创建r3用的选择子：代码段、数据段
    r3_code_selector = 4 << 3 | 0b011;                         // 索引为4，请求特权级为 r3
    r3_data_selector = 5 << 3 | 0b011;                         // 索引为5，请求特权级为 r3

    gdt_ptr.base = &gdt;
    gdt_ptr.limit = sizeof(gdt) - 1;

    __asm__ volatile ("lgdt gdt_ptr;");                        // 重新加载 gdt 表
}