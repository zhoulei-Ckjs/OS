#include "../include/linux/mm.h"

#define ARDS_ADDR   0x1100

void print_check_memory_info()
{
    check_memory_info_t* p = (check_memory_info_t*)ARDS_ADDR;
    check_memmory_item_t* p_data = (check_memmory_item_t*)(ARDS_ADDR + 2);

    unsigned short times = p->times;

    for (int i = 0; i < times; ++i)
    {
        check_memmory_item_t* tmp = p_data + i;
        printk("%x, %x, %x, %x, %d\n", tmp->base_addr_high, tmp->base_addr_low,
               tmp->length_high, tmp->length_low, tmp->type);
    }
}
