#include "../include/linux/task.h"
#include "../include/string.h"

/**
 * @brief 所有的 task 指针
 */
task_t* tasks[NR_TASKS] = {0};

/**
 * @brief 从所有 task 中找寻空闲的 task
 * @return 找到进程返回进程号（>0）
 *         所有进程用光则返回 -1
 */
static int find_empty_process()
{
    int ret = 0;
    bool is_finded = false;

    for (int i = 0; i < NR_TASKS; ++i)
    {
        if (0 == tasks[i])
        {
            is_finded = true;
            ret = i;
            break;
        }
    }

    if (!is_finded)
    {
        printk("no valid pid\n");
        return -1;
    }

    return ret;
}

/**
 * @brief 创建进程
 * @return 进程描述信息
 */
task_t* create_task()
{
    task_union_t* task = (task_union_t*)get_free_page();
    memset(task, 0, PAGE_SIZE);

    task->task.pid = find_empty_process();      ///< 找空闲的进程
    tasks[task->task.pid] = &(task->task);      ///< 进程栈空间分给当前 task

    /**
     * 每个进程都要有自己的虚拟页表
     */
    task->task.cr3 = virtual_memory_init();
    return task;
}

void task_init()
{
    create_task();
}
