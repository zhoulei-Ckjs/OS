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

    STI        ///< 开中断

    while (true);
}