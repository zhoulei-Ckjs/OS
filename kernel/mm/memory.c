#include "linux/kernel.h"
#include "linux/mm.h"

#define ARDS_TIMES_BUFFER 0x8200

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