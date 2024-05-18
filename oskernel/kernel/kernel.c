#include "../include/linux/kernel.h"

inline void set_cr3(uint v)
{
    asm volatile("mov cr3, eax;" ::"a"(v));
}

inline void enable_page()
{
    asm volatile("mov eax, cr0;"
                 "or eax, 0x80000000;"      ///< 1000 0000 0000 0000 0000 0000 0000 0000（第 32 位为 1）
                 "mov cr0, eax;");
}