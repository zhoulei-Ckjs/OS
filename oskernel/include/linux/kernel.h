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
 *
 * CR3寄存器是x86架构CPU中的一个控制寄存器，它存储了当前任务的页目录表（Page Directory Table）的物理地址。在使用虚拟内存时，操作系统将
 * 整个物理内存映射到一段连续的虚拟地址空间上，每个进程都有自己的虚拟地址空间，而CR3寄存器则用于记录当前执行的进程所使用的页目录表的物理地
 * 址。当进程需要访问某个虚拟地址时，操作系统会根据这个虚拟地址通过页表查找得到相应的物理地址，并将其转换为物理地址进行访问。因此，CR3寄存
 * 器在虚拟内存管理中起着至关重要的作用。\n
 *
 * @attention 需要注意，cr3指向 PDT 的物理地址，而不是虚拟地址（在启用分页后，PDT表也被映射到一个虚拟地址，如 PDT 的物理地址为 0x100000，然后我们
 * 映射虚拟地址 0x200000 为物理地址 0x100000，然后我们访问 0x200000 就会访问 0x100000，但是进程 cr3 的值还是 0x100000，即物理地址）
 * @param v     新 cr3 的值
 */
void set_cr3(uint v);

/**
 * @brief 启用分页机制，修改 cr0 的 PG 位为 1，即开启分页
 */
void enable_page();

#endif //HAINIU_KERNEL_H
