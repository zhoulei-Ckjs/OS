#include "../include/linux/tty.h"
#include "../include/linux/traps.h"
#include "../include/asm/system.h"
#include "../include/linux/mm.h"
#include "../include/linux/task.h"
#include "../include/linux/sched.h"

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

    task_init();                ///< 进程初始化，虚拟页表
    sched();
    kmalloc(1);            ///< 测试分配虚拟内存

    STI        ///< 开中断

    while (true);
}