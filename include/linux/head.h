#ifndef OS_HEAD_H
#define OS_HEAD_H

/**
 * @brief 定义存储 gdt 信息的结构体
 */
#pragma pack(2)
typedef struct global_descriptor_table_pointer
{
    short limit;        ///< 界限（表的字节数 - 1）
    int base;           ///< gdt 表存储的地址
}gdt_ptr_t;
#pragma pack()

#endif //OS_HEAD_H
