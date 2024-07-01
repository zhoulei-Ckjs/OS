#include "../include/linux/types.h"

/**
 * @brief 桶链表结构
 */
typedef struct bucket_desc
{
    void                    *page;         ///< 管理的物理页
    struct bucket_desc      *next;         ///< 链表，下一个bucket地址
    void                    *freeptr;      ///< 下一个可供分配的
    unsigned short		    refcnt;        ///< 引用计数，释放物理页时要用
    unsigned short		    bucket_size;   ///< 每个桶的大小
} bucket_desc_t;

/**
 * @brief 桶链表
 */
typedef struct _bucket_dir
{
    int			            size;          ///< 桶大小
    bucket_desc_t           *chain;        ///< 链表
} bucket_dir_t;

bucket_dir_t bucket_dir[] = {
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
    void* ret = NULL;

    bucket_dir_t bucket_dir_item;
    return ret;
}