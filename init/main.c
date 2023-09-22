#include "linux/tty.h"
#include "linux/traps.h"
#include "sys/types.h"
#include "asm/system.h"
#include "linux/mm.h"
#include "linux/kernel.h"

void main(void)
{
    console_init();     ///< 控制台初始化
    gdt_init();         ///< 初始化 gdt 表
    idt_init();         ///< 初始化中断
    clock_init();       ///< 初始化时钟

    print_check_memory_info();
    memory_init();

    for (int i = 0; i < 3; ++i)
    {
        void* p = get_free_page();
        free_page(p);
    }

    STI                 ///< 启用中断
    while(true);
}