#include "../include/linux/tty.h"
#include "../include/linux/kernel.h"
#include "../include/string.h"
#include "../include/linux/traps.h"
#include "../include/asm/system.h"
#include "../include/linux/mm.h"

extern void clock_init();

void kernel_main(void)
{
    console_init();
    gdt_init();
    idt_init();
    clock_init();

    print_check_memory_info();
    memory_init();
    memory_map_int();

    for (int i = 0; i < 3; ++i)
    {
        void* p = get_free_page();
        printk("find page %p\n", p);
    }

    STI        ///< 开中断

    while (true);
}