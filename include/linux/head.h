#ifndef OS_HEAD_H
#define OS_HEAD_H

/**
 * @brief 段描述符
 */
typedef struct gdt_item
{
    unsigned short limit_low;           ///< 段界限 0 ~ 15 位
    unsigned int base_low : 24;         ///< 基地址 0 ~ 23 位 16M
    unsigned char type : 4;             ///< 段类型
    unsigned char segment : 1;          ///< 1 表示代码段或数据段，0 表示系统段
    unsigned char DPL : 2;              ///< Descriptor Privilege Level 描述符特权等级 0 ~ 3
    unsigned char present : 1;          ///< 存在位，1 在内存中，0 在磁盘上
    unsigned char limit_high : 4;       ///< 段界限 16 ~ 19
    unsigned char available : 1;        ///< 该位是用户位，可以被用户自由使用。
    unsigned char long_mode : 1;        ///< 64 位扩展标志
    unsigned char big : 1;              ///< 该位为 0 表示这是一个 16 位的段，1 表示这是一个 32 位段。
    unsigned char granularity : 1;      ///< 粒度 4KB 或 1B，0 表示以字节为单位，20根地址总线就是 2 ^ 20 = 1M；1表示以4K为单位，20跟地址总线就是1M * 4K = 4G。
    unsigned char base_high;            ///< 基地址 24 ~ 31 位
} __attribute__((packed)) gdt_item_t;   ///< 按紧凑方式对结构体进行内存对齐，取消默认的对齐填充

/**
 * @brief 定义存储 gdt 信息的结构体
 */
#pragma pack(2)
typedef struct global_descriptor_table_pointer
{
    short limit;        ///< 界限（表的字节数 - 1）
    gdt_item_t* base;   ///< gdt 表存储的地址
}gdt_ptr_t;
#pragma pack()

/**
 * @brief 中断描述符
 */
typedef struct interrupt_descriptor
{
    short offset0;                                  ///< 段内偏移 0 ~ 15 位
    short selector;                                 ///< 代码段选择子
    char reserved;                                  ///< 保留不用
    char type : 4;                                  ///< 任务门/中断门/陷阱门
    char segment : 1;                               ///< segment = 0 表示系统段
    char DPL : 2;                                   ///< 使用 int 指令访问的最低权限
    char present : 1;                               ///< 是否有效
    short offset1;                                  ///< 段内偏移 16 ~ 31 位
} __attribute__((packed)) interrupt_descriptor;     ///< 按紧凑方式对结构体进行内存对齐，取消默认的对齐填充

#endif //OS_HEAD_H
