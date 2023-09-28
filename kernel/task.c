#include "linux/task.h"
#include "string.h"

void create_task()
{
    task_union_t* task = (task_union_t*)get_free_page();
    memset(task, 0, PAGE_SIZE);
    task->task.cr3 = virtual_memory_init();     ///< 每个进程都要有自己的虚拟页表
}

void task_init()
{
    create_task();
}