#ifndef OS_STRING_H
#define OS_STRING_H

#include "sys/types.h"

/**
 * @brief 计算字符串长度，'\0'除外。
 * @param str 待计算字符串
 * @return 长度
 */
size_t strlen(const char* str);

/**
 * @brief 内存拷贝函数
 * @param dest 目标地址
 * @param src 源地址
 * @param count 拷贝字节数
 * @return 目的地址
 */
void* memcpy(void *dest, void *src, size_t count);

/**
 * @brief 内存设定为指定字符
 * @param dest 目标内存
 * @param ch 要设定的值
 * @param count 长度
 * @return dest
 */
void* memset(void* dest, char ch, size_t count);

#endif //OS_STRING_H