#include "../include/linux/mm.h"
#include "../include/asm/system.h"
#include "../include/linux/kernel.h"

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
            /// 第一块映射区，给内核用，每个结构体 4B，4K 能够存储 0X400 个结构体
            for (int j = 0; j < 0x400; ++j)
            {
                int* item = &ptt_arr[j];

                int virtual_addr = j * 0x1000;      ///< 地址为 4K 的整数
                /// 00000000000000000000000000000_普通用户也可以访问_可读写_物理页有效（物理页不存在，访问会发生缺页中断）
                *item = 0b00000000000000000000000000000111 | virtual_addr;
            }
        }
        else
        {
            for (int j = 0; j < 0x400; ++j)
            {
                int* item = &ptt_arr[j];

                int virtual_addr = j * 0x1000;
                virtual_addr = virtual_addr + i * 0x400 * 0x1000;   ///< 0x400 * 0x1000 是跳过 i==0 时分配的地址。

                /// 00000000000000000000000000000_普通用户也可以访问_可读写_物理页有效
                *item = 0b00000000000000000000000000000111 | virtual_addr;
            }
        }
    }

    BOCHS_DEBUG_MAGIC

    set_cr3((uint)pdt);
    enable_page();

    BOCHS_DEBUG_MAGIC
}