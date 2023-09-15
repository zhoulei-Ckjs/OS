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
    /// 内存类型 1：通常表示系统检测到的主内存模块，可能是主板上默认的内存或主存储器。
    /// 内存类型 2：可能表示系统检测到的外部内存扩展模块，例如插槽中安装的扩展内存模块。
    unsigned int  type;             ///< 描述内存块的类型
}check_memmory_item_t;

/**
 * @brief 内存块检测信息
 */
typedef struct check_memory_info
{
    unsigned short          times;  ///< 多少块内存
    check_memmory_item_t*   data;   ///< 第一个块内存的指针
}check_memory_info_t;

/**
 * @brief 打印内存检测信息
 */
void print_check_memory_info();

#endif //OS_MM_H
