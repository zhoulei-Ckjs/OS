#ifndef OS_KERNEL_H
#define OS_KERNEL_H

#include "stdarg.h"

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

#endif //OS_KERNEL_H
