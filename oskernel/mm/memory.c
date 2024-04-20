#include "../include/linux/mm.h"

#define ARDS_ADDR                   0x1100

/// 把1M以下内存称为无效内存
#define VALID_MEMORY_FROM           0x100000

physics_memory_info_t g_physics_memory;

void print_check_memory_info()
{
    check_memory_info_t* p = (check_memory_info_t*)ARDS_ADDR;
    check_memmory_item_t* p_data = (check_memmory_item_t*)(ARDS_ADDR + 2);

    unsigned short times = p->times;

    printk("addr:\n");

    for (int i = 0; i < times; ++i)
    {
        check_memmory_item_t* tmp = p_data + i;
        printk("type=%d, addr_start=%08x%08x + %08x%08x\n", tmp->type, tmp->base_addr_high, tmp->base_addr_low,
               tmp->length_high, tmp->length_low);
    }
}

void memory_init()
{
    check_memory_info_t* p = (check_memory_info_t*)ARDS_ADDR;
    check_memmory_item_t* p_data = (check_memmory_item_t*)(ARDS_ADDR + 2);

    for (int i = 0; i < p->times; ++i)
    {
        check_memmory_item_t* tmp = p_data + i;
        if (tmp->base_addr_low > 0 && tmp->type == ZONE_VALID)
        {
            g_physics_memory.addr_start = tmp->base_addr_low;
            g_physics_memory.valid_mem_size = tmp->length_low;
            g_physics_memory.addr_end = tmp->base_addr_low + tmp->length_low;
        }
    }

    /// 如果没有可用内存
    if (VALID_MEMORY_FROM != g_physics_memory.addr_start)
    {
        printk("[%s:%d] no valid physics memory\n", __FILE__, __LINE__);
        return;
    }

    g_physics_memory.pages_total = g_physics_memory.addr_end >> 12;     ///< 4096 byte 为 1 页
    g_physics_memory.pages_used = 0;
    g_physics_memory.pages_free = g_physics_memory.pages_total - g_physics_memory.pages_used;
}