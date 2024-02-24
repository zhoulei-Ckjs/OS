#include "../include/linux/tty.h"
#include "../include/linux/kernel.h"
#include "../include/string.h"
#include "../include/linux/traps.h"

void kernel_main(void)
{
    console_init();
    gdt_init();
    idt_init();

    __asm__("sti;");        ///< 开中断

//    int i = 10 / 0;         ///< 除零异常

    while (true);
}