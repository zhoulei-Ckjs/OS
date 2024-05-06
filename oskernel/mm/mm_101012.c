#include "../include/linux/mm.h"

void virtual_memory_init()
{
    int* pdt = (int*)get_free_page();       ///< PDT表
    /// 清零
    memset(pdt, 0, PAGE_SIZE);

    for (int i = 0; i < 4; ++i)
    {
        /// pdt里面的每项，即pde，内容是ptt页表地址 + 尾12位的权限位
        int ptt = (int) get_free_page();
        /// 00000000000000000000000000000_普通用户也可以访问_可读写_物理页有效
        int pde = 0b00000000000000000000000000000111 | ptt;
        pdt[i] = pde;

        int* ptt_arr = (int*)ptt;

        if (0 == i)
        {
            /// 第一块映射区，给内核用
            for (int j = 0; j < 0x400; ++j)
            {
                int* item = &ptt_arr[j];

                int virtual_addr = j * 0x1000;
                /// 00000000000000000000000000000_普通用户也可以访问_可读写_物理页无效（物理页不存在，访问会发生缺页中断）
                *item = 0b00000000000000000000000000000110 | virtual_addr;
            }
        }
    }
}