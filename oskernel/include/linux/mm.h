#ifndef OS_MM_H
#define OS_MM_H

#include "types.h"

/**
 * @brief 内存块结构
 */
typedef struct
{
    unsigned int  base_addr_low;    ///< 内存基地址的低32位
    unsigned int  base_addr_high;   ///< 内存基地址的高32位
    unsigned int  length_low;       ///< 内存块长度的低32位
    unsigned int  length_high;      ///< 内存块长度的高32位
    /**
     * 描述内存块的类型
     * - 1 表示一个内存区域是可用的，也就是这部分内存是健康的、未被占用，可以被操作系统或应用程序使用
     * - 2 表示一个内存区域是保留的或不可用的，这可能是因为这部分内存已经被某些硬件或固件占用了
     */
    unsigned int  type;
}check_memmory_item_t;

#define ZONE_VALID 1        ///< ards 可用内存区域（Address Range Descriptor Structure）

/**
 * @brief 内存信息
 */
typedef struct
{
    unsigned short          times;
    check_memmory_item_t*   data;
}check_memory_info_t;

/**
 * @brief 打印内存信息
 */
void print_check_memory_info();

#endif //OS_MM_H
