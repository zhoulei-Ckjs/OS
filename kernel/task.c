#include "linux/mm.h"

void create_task()
{
    /**
     * 每个进程都要有自己的虚拟页表
     */
    virtual_memory_init();
}

void task_init()
{
    create_task();
}