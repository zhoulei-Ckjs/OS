#include "linux/task.h"
#include "string.h"
#include "linux/kernel.h"

/**
 * @brief 所有task指针
 */
task_t* tasks[NR_TASKS] = {0};

/**
 * @brief 寻找空闲task
 * @return 进程号
 * @retval -1 无可用进程
 */
static int find_empty_process()
{
    int ret = 0;
    bool is_find = false;

    for (int i = 0; i < NR_TASKS; ++i)
    {
        if (0 == tasks[i])
        {
            is_find = true;
            ret = i;
            break;
        }
    }

    if (!is_find)
    {
        printk("no valid pid!!!\n");
        return -1;
    }

    return ret;
}

task_t* create_task()
{
    task_union_t* task = (task_union_t*)get_free_page();
    memset(task, 0, PAGE_SIZE);

    task->task.pid = find_empty_process();      ///< 找空闲的进程
    tasks[task->task.pid] = &(task->task);      ///< 进程栈空间分给当前task
    task->task.cr3 = virtual_memory_init();     ///< 每个进程都要有自己的虚拟页表

    return &(task->task);
}

void task_init()
{
    create_task();
}