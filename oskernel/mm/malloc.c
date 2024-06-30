#include "../include/linux/types.h"

/**
 * @brief 桶
 */
typedef struct bucket_desc
{
    void *                  page;          ///< 管理的物理页
    struct bucket_desc *    next;          ///< 下一个bucket地址
    void *                  freeptr;       ///< 下一个可供分配的
    unsigned short		    refcnt;        ///< 引用计数，释放物理页时要用
    unsigned short		    bucket_size;   ///< 每个桶的大小
} bucket_desc_t;

/**
 * @brief 桶链表
 */
typedef struct _bucket_dir
{
    int			            size;           ///< 大小
    struct bucket_desc_t *  chain;
} bucket_dir_t;

void* kmalloc(size_t len)
{
    void* ret = NULL;

    bucket_dir_t bucket_dir_item;
    return ret;
}