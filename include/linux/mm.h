#ifndef OS_MM_H
#define OS_MM_H

#include "sys/types.h"

/**
 * @brief 内存块结构
 */
typedef struct check_memmory_item
{
    unsigned int  base_addr_low;    ///< 内存基地址的低 32 位
    unsigned int  base_addr_high;   ///< 内存基地址的高 32 位
    unsigned int  length_low;       ///< 内存块长度的低 32 位
    unsigned int  length_high;      ///< 内存块长度的高 32 位

    /// 描述内存块的类型。
    /// 内存类型 1：
    ///     AddressRangeMemory，这是可用的、正常的 RAM。
    ///     操作系统可以自由地读写这些内存区域，并将其分配给应用程序或内核使用。
    /// 内存类型 2：
    ///     AddressRangeReserved，这些是不可用的、被硬件保留的内存区域。
    unsigned int  type;
}check_memmory_item_t;

#define ZONE_VALID 1                ///< 可用的、正常的内存。

/**
 * @brief 内存块检测信息
 */
typedef struct check_memory_info
{
    unsigned short          times;  ///< 多少块内存
    check_memmory_item_t*   data;   ///< 第一个块内存的指针
}check_memory_info_t;

/**
 * @brief 描述一块物理内存的结构
 */
typedef struct physics_memory_info
{
    uint addr_start_;               ///< 可用内存起始地址 一般是 1 M
    uint addr_end_;                 ///< 可用内存结束地址
    uint valid_mem_size_;           ///< 可用内存大小
}physics_memory_info_t;

/**
 * @brief 打印内存检测信息
 */
void print_check_memory_info();

/**
 * @brief 内存初始化
 */
void memory_init();

#endif //OS_MM_H
