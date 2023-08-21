#ifndef OS_TRAPS_H
#define OS_TRAPS_H

/**
 * @brief 初始化 gdt 表。
 */
void gdt_init();

/**
 * @brief 初始化中断向量表
 */
void idt_init();

#endif //OS_TRAPS_H
