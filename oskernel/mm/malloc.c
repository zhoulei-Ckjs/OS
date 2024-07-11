#include "../include/linux/types.h"
#include "../include/linux/kernel.h"
#include "../include/linux/mm.h"

/**
 * @brief 桶结构体描述符，16字节
 *
 * 每个桶结构体描述符管理一个物理页（4096KB）;
 * 其中 bucket_size 为当前桶的大小，如 16K，则一共可以分配 4096/16 = 256 次;
 * 其中 refcnt 统计了本页有多少个小块内存被分配出去了，只有当 refcnt == 0 时，我们才能将这个桶结构体所管理的物理页释放。
 */
struct bucket_desc
{
    void*                   page;          ///< 管理的物理页
    struct bucket_desc*     next;          ///< 链表，下一个bucket地址
    void*                   freeptr;       ///< 下一个可供分配的
    unsigned short		    refcnt;        ///< 引用计数，释放物理页时要用，只有当 refcnt == 0 时才能释放所管理的物理页
    unsigned short		    bucket_size;   ///< 每个桶的大小
};

/**
 * @brief 桶链表
 */
struct _bucket_dir
{
    int			            size;          ///< 桶大小
    struct bucket_desc*     chain;         ///< 链表
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

/// 空闲桶链表
struct bucket_desc *free_bucket_desc = (struct bucket_desc *) 0;

/**
 * @brief 初始化桶描述符
 *
 * 我们专门申请一页内存来存储桶描述符，然后用这些桶描述符来管理内存
 */
static void init_bucket_desc()
{
    struct bucket_desc *bdesc, *first;

    /// TODO get_free_page 不能访问内存了
    first = bdesc = (struct bucket_desc *) get_free_page();
    if (!bdesc)
    {
        printk("[Error]: get_free_page return null\n");
        return;
    }
    /// 将桶链表链接起来
    for (int i = PAGE_SIZE/sizeof(struct bucket_desc); i > 1; i--)
    {
        bdesc->next = bdesc+1;
        bdesc++;
    }
    bdesc->next = free_bucket_desc;
    free_bucket_desc = first;
}

void* kmalloc(size_t len)
{
    struct _bucket_dir*     bdir    = NULL;                 ///< 桶页
    struct bucket_desc*     bdesc   = NULL;
    void*                   ret     = NULL;
    int                     size    = 0;                    ///< 辅助统计

    /// 找到存储桶页
    for (bdir = bucket_dir; bdir->size; bdir++)
        if (bdir->size >= len)
            break;
    /// 找到了结尾符，说明要申请的空间太大了
    if (!bdir->size)
    {
        printk("malloc called with impossibly large argument (%d)\n", len);
        return NULL;
    }

    /// 找一块空闲空间来分配
    for (bdesc = bdir->chain; bdesc; bdesc = bdesc->next)
        if (bdesc->freeptr)
            break;

    /// 如果没找到空间
    if (!bdesc)
    {
        if (!free_bucket_desc)
            init_bucket_desc();     ///< 初始化桶描述符表

        printk("no space left, need to allocate!\n", len);
        return NULL;
    }
    return ret;
}