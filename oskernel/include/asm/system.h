#ifndef HAINIU_SYSTEM_H
#define HAINIU_SYSTEM_H

#define BOCHS_DEBUG_MAGIC   __asm__("xchg bx, bx");

#define CLI   __asm__("cli");       ///< 关闭中断
#define STI   __asm__("sti");       ///< 开中断

#endif //HAINIU_SYSTEM_H
