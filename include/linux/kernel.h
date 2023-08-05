#ifndef OS_KERNEL_H
#define OS_KERNEL_H

/**
 * @brief 内核打印函数
 * @param format 格式
 * @param ... 可变参
 * @return 打印的字符串长度
 */
int printk(const char* format, ...);

#endif //OS_KERNEL_H
