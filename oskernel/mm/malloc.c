#include "../include/linux/types.h"
#include "../include/linux/kernel.h"
#include "../include/linux/mm.h"

/**
 * @brief 桶链表结构，16字节
 */
struct bucket_desc
{
    void*                   page;          ///< 管理的物理页
    struct bucket_desc*     next;          ///< 链表，下一个bucket地址
    void*                   freeptr;       ///< 下一个可供分配的
    unsigned short		    refcnt;        ///< 引用计数，释放物理页时要用
    unsigned short		    bucket_size;   ///< 每个桶的大小
};

/**
 * @brief 桶链表
 */
struct _bucket_dir
{
    int			            size;          ///< 桶大小
    struct bucket_desc*     chain;        ///< 链表
};

/**
 * @brief 不同大小的桶
 */
struct _bucket_dir bucket_dir[] =
{
    /// 参考嵌入式指针，一块空间在没有被分配时，当前链表指使用。
    /// 而链表结构为 bucket_desc_t， 所以最小桶大小 >= 16，只适用于 32 位系统
    /// 64 位系统由于指针变大，而造成 16 字节放不下嵌入式指针
    {16,   (struct bucket_desc *) 0},       ///< 16字节存储桶
    {32,   (struct bucket_desc *) 0},
    {64,   (struct bucket_desc *) 0},
    {128,  (struct bucket_desc *) 0},
    {256,  (struct bucket_desc *) 0},
    {512,  (struct bucket_desc *) 0},
    {1024, (struct bucket_desc *) 0},
    {2048, (struct bucket_desc *) 0},
    {4096, (struct bucket_desc *) 0},
    {0,    (struct bucket_desc *) 0}        ///< 结尾符
};

void* kmalloc(size_t len)
{
    struct _bucket_dir*     bdir    = NULL;
    void*                   ret     = NULL;
    int                     size    = 0;                    ///< 辅助统计

    /// 找到存储桶
    for (bdir = bucket_dir; bdir->size; bdir++)
        if (bdir->size >= len)
            break;

    /// 如果没找到桶
    if (0 == bdir->size)
    {
        printk("no proper bucket: %d\n", len);
        return NULL;
    }

    return ret;
}