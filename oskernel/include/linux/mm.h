#ifndef OS_MM_H
#define OS_MM_H

#include "types.h"

#define PAGE_SIZE 4096              ///< 单个内存页的大小

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
    unsigned short          times;      ///< 有多少块内存块结构
    check_memmory_item_t*   data;
}check_memory_info_t;

/**
 * @brief 描述一块物理内存的结构
 */
typedef struct
{
    uint    addr_start;     ///< 可用内存起始地址 一般是1M
    uint    addr_end;       ///< 可用内存结束地址
    uint    valid_mem_size; ///< 可用内存大小
    uint    pages_total;    ///< 机器物理内存共多少page
    uint    pages_free;     ///< 机器物理内存还剩多少page
    uint    pages_used;     ///< 机器物理内存用了多少page
}physics_memory_info_t;

typedef struct
{
    uint            addr_base;          ///< 可用物理内存开始位置  3M
    uint            pages_total;        ///< 共有多少page   机器物理内存共多少page - 0x30000（3M）
    uint            bitmap_item_used;   ///< 如果1B映射一个page，用了多少个page
    uchar*          map;
}physics_memory_map_t;

/**
 * @brief 打印内存信息
 */
void print_check_memory_info();

/**
 * @brief 内存初始化
 */
void memory_init();

/**
 * @brief 初始化内存map
 */
void memory_map_int();

#endif //OS_MM_H
