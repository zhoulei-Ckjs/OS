#ifndef OS_TASK_H
#define OS_TASK_H

#include "mm.h"

/**
 * @brief 进程描述信息
 */
typedef struct task_t
{
    int pid;        ///< 进程 id
    int cr3;        ///< 存储页表起始地址
}task_t;

/**
 * @brief 进程
 */
typedef union task_union_t
{
    task_t task;                ///< 进程描述信息
    char stack[PAGE_SIZE];      ///< 进程栈空间
}task_union_t;

/**
 * @brief 进程初始化
 */
void task_init();

#endif //OS_TASK_H
