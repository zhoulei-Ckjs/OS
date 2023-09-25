#ifndef OS_KERNEL_H
#define OS_KERNEL_H

#include "stdarg.h"
#include "sys/types.h"

/**
 * @brief 内核打印函数
 * @param format 格式
 * @param ... 可变参
 * @return 打印的字符串长度
 */
int printk(const char* format, ...);

/**
 * @brief 拼接字符串函数
 * @param buf 输出内存
 * @param fmt 格式
 * @param args 参数列表
 * @return 总字符串长度
 */
int vsprintf(char* buf, const char* fmt, va_list args);

/**
 * @brief 修改 cr3 寄存器的值
 * CR3 寄存器是 x86 架构 CPU 中的一个控制寄存器，它存储了当前任务的页目录表（Page Directory Table）的物理地址。
 * 在使用虚拟内存时，操作系统将整个物理内存映射到一段连续的虚拟地址空间上，每个进程都有自己的虚拟地址空间，
 * 而 CR3 寄存器则用于记录当前执行的进程所使用的页目录表的物理地址。
 * 当进程需要访问某个虚拟地址时，操作系统会根据这个虚拟地址通过页表查找得到相应的物理地址，并将其转换为物理地址进行访问。
 * @param cr3 新 cr3 的值
 */
void set_cr3(uint cr3);

/**
 * @brief 启用分页机制，修改 cr0 的 PG 位为 1，即开启分页
 */
void enable_page();

#endif //OS_KERNEL_H
