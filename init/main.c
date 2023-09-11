#include "linux/tty.h"
#include "linux/traps.h"
#include "sys/types.h"

void main(void)
{
    console_init();     ///< 控制台初始化
    gdt_init();         ///< 初始化 gdt 表
    idt_init();         ///< 初始化中断
    clock_init();       ///< 初始化时钟

    __asm__("sti;");    ///< 启用中断
    while(true);
}