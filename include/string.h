#ifndef OS_STRING_H
#define OS_STRING_H

#include "sys/types.h"

/**
 * @brief 计算字符串长度，'\0'除外。
 * @param str 待计算字符串
 * @return 长度
 */
size_t strlen(const char* str);

#endif //OS_STRING_H