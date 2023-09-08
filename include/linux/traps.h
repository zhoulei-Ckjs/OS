#ifndef OS_TRAPS_H
#define OS_TRAPS_H

#include "linux/head.h"

/**
 * @brief 初始化 gdt 表。
 */
void gdt_init();

/**
 * @brief 初始化中断向量表
 */
void idt_init();

/**
 * @brief 发送 end of interrupt，当中断处理完成后，必须向 PIC 发送 EOI 命令，
 * 否则PIC会认为中断仍在处理中，不会发送新的中断。
 * @param idt_index
 */
void send_eoi(int idt_index);

/**
 * @brief 写 gdt、idt 表信息。
 * @param p gdt、idt 表指针。
 * @param limit 长度（字节），一般为 sizeof(gdt/idt) - 1，因为 base + limit 为最后一个字节。
 * @param base 基地址。
 */
void write_xdt_ptr(xdt_ptr_t* p,  int base, short limit);

#endif //OS_TRAPS_H
