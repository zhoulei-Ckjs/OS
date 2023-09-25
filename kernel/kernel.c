#include "linux/kernel.h"

void set_cr3(uint cr3)
{
    asm volatile("mov cr3, eax;" ::"a"(cr3));
}

void enable_page()
{
    asm volatile("mov eax, cr0;"
                 "or eax, 0x80000000;"      ///< 1000 0000 0000 0000 0000 0000 0000 0000（第32位PG位为1）
                 "mov cr0, eax;");
}