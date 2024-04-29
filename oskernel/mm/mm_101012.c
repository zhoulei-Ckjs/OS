#include "../include/linux/mm.h"

void virtual_memory_init()
{
    int* pdt = (int*)get_free_page();       ///< PDT表
    /// 清零
    memset(pdt, 0, PAGE_SIZE);

    /// pdt里面的每项，即pde，内容是ptt页表地址还是索引？ + 尾12位的权限位
    int ptt = (int)get_free_page();
    /// 00000000000000000000000000000_普通用户也可以访问_可读写_物理页有效
    int pde = 0b00000000000000000000000000000111 | ptt;
    pdt[0] = pde;
}