#ifndef OS_SYSTEM_H
#define OS_SYSTEM_H

/// bochs 的魔术断点
#define BOCHS_DEBUG_MAGIC __asm__("xchg bx, bx");

#define CLI __asm__("cli");           ///< 关闭中断
#define STI __asm__("sti");           ///< 开中断

#endif //OS_SYSTEM_H
