#include "linux/kernel.h"
#include "linux/mm.h"

#define ARDS_TIMES_BUFFER 0x8200

/**
 * @brief 物理内存
 */
physics_memory_info_t g_physics_memory;

void print_check_memory_info()
{
    check_memory_info_t* p = (check_memory_info_t*)ARDS_TIMES_BUFFER;
    check_memmory_item_t* p_data = (check_memmory_item_t*)(ARDS_TIMES_BUFFER + 2);

    unsigned short times = p->times;

    printk("memory check result:\n");
    printk("    base_higt    base_low   length_high  length_low   type\n");

    for (int i = 0; i < times; ++i)
    {
        check_memmory_item_t* tmp = p_data + i;
        printk("%10x, %10x, %10x, %12x, %6d\n", tmp->base_addr_high, tmp->base_addr_low,
               tmp->length_high, tmp->length_low, tmp->type);
    }
}

void memory_init()
{
    unsigned short* times = (unsigned short*)ARDS_TIMES_BUFFER;     ///< 内存块个数
    check_memmory_item_t* data = (check_memmory_item_t*)(ARDS_TIMES_BUFFER + 2);    ///< 内个存块地址
    for (int i = 0; i < *times; ++i)
    {
        check_memmory_item_t* tmp = data + i;
        if(tmp->base_addr_low > 0 && tmp->type == ZONE_VALID)
        {
            g_physics_memory.addr_start_ = tmp->base_addr_low;
            g_physics_memory.valid_mem_size_ = tmp->length_low;
            g_physics_memory.addr_end_ = tmp->base_addr_low + tmp->length_low;
        }
    }

    /// 我们只使用 1M 以上的内存，检测出的结果应该只有一块有效内存在 1M 以上，所以我们判断
    /// 是不是起始位置为 1M 就可以了。
    if (0x100000 != g_physics_memory.addr_start_)
    {
        printk("[%s:%d] no valid physics memory\n", __FILE__, __LINE__);
        printk("addr_start = %x\n", g_physics_memory.addr_start_);
        printk("addr_end = %x\n", g_physics_memory.addr_end_);
        printk("size = %x\n", g_physics_memory.valid_mem_size_);
        return;
    }

    /// 初始化物理内存
    g_physics_memory.pages_total_ = g_physics_memory.valid_mem_size_ >> 12;     ///< 4096 byte 为 1 页
    g_physics_memory.pages_used_ = 0;
    g_physics_memory.pages_free_ = g_physics_memory.pages_total_ - g_physics_memory.pages_used_;
}