/**
 * @brief 类型定义
 */

#ifndef OS_TYPES_H
#define OS_TYPES_H

#define EOS '\0'                        ///< 字符串结尾
#define NULL ((void *)0)                ///< 空指针

#define true 1
#define false 0

#define bool _Bool                      ///< _Bool 是 C 语言中的布尔数据类型

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int size_t;
typedef unsigned int uint;
typedef unsigned long long u64;

#endif //OS_TYPES_H
