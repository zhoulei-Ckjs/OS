#include "../include/linux/tty.h"
#include "../include/linux/kernel.h"
#include "../include/string.h"
#include "../include/linux/traps.h"

extern void clock_init();

void kernel_main(void)
{
    console_init();
    gdt_init();
    idt_init();
    clock_init();

    __asm__("sti;");        ///< 开中断

    while (true);
}