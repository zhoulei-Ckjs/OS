#ifndef HAINIU_TRAPS_H
#define HAINIU_TRAPS_H

#include "head.h"

/**
 * @brief 初始化 gdt 表
 */
void gdt_init();

/**
 * @brief 初始化中断向量表 idt
 */
void idt_init();

/**
 * @brief 发送 end of interrupt
 * @param idt_index
 */
void send_eoi(int idt_index);

/**
 * @brief 写入中断描述表 大小，在 IDT 表加载时需要告诉 cpu 表大小是多少，基地址是什么
 * @param p         描述符表
 * @param limit     大小（字节）
 * @param base      基地址
 */
void write_xdt_ptr(xdt_ptr_t* p, short limit, int base);

#endif //HAINIU_TRAPS_H
