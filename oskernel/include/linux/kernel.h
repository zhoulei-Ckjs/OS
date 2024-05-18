#ifndef HAINIU_KERNEL_H
#define HAINIU_KERNEL_H

#include "../stdarg.h"
#include "types.h"

/**
 * 内核打印函数
 * @param fmt 打印格式
 * @param ... 可变参
 * @return
 */
int printk(const char * fmt, ...);

/**
 * 组装字符串函数
 * @param buf   格式化字符串存储的内存地址
 * @param fmt   格式
 * @param args  参数的指针类型（转换为了 char* 类型），使用的时候需要将 char* 转换为相应的数据类型
 * @return      字符串总长度
 */
int vsprintf(char *buf, const char *fmt, va_list args);

/**
 * @brief 修改 cr3 寄存器的值
 * CR3寄存器是x86架构CPU中的一个控制寄存器，它存储了当前任务的页目录表（Page Directory Table）的物理地址。在使用虚拟内存时，操作系统将
 * 整个物理内存映射到一段连续的虚拟地址空间上，每个进程都有自己的虚拟地址空间，而CR3寄存器则用于记录当前执行的进程所使用的页目录表的物理地
 * 址。当进程需要访问某个虚拟地址时，操作系统会根据这个虚拟地址通过页表查找得到相应的物理地址，并将其转换为物理地址进行访问。因此，CR3寄存
 * 器在虚拟内存管理中起着至关重要的作用。
 * @param v     新 cr3 的值
 */
void set_cr3(uint v);

/**
 * @brief 启用分页机制，修改 cr0 的 PG 位为 1，即开启分页
 */
void enable_page();

#endif //HAINIU_KERNEL_H
