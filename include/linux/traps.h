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

/**
 * @brief 发送 end of interrupt，当中断处理完成后，必须向 PIC 发送 EOI 命令，
 * 否则PIC会认为中断仍在处理中，不会发送新的中断。
 * @param idt_index
 */
void send_eoi(int idt_index);

#endif //OS_TRAPS_H
