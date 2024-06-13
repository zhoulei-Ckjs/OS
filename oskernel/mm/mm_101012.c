#include "../include/linux/mm.h"
#include "../include/asm/system.h"
#include "../include/linux/kernel.h"

void* virtual_memory_init()
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
            /// 第一块映射区，给内核用，每个结构体 4B，4K 能够存储 0X400 即 1024 个结构体
            /**
             * 这里可以映射 4M 地址空间，一个满的 pdt 表有 4096B / 4B = 1024 个 ptt 表
             * ptt 表中每一项 pte 都代表了 4096B
             * 所以这里总共映射为：1024 × 4096B = 4M
             */
            for (int j = 0; j < 0x400; ++j)
            {
                int* item = &ptt_arr[j];

                int actual_addr = j * 0x1000;      ///< 地址为 4K 的整数
                /// TODO 下面这两种写法为什么会失败？
//                int actual_addr = j * 0x1000 + 0x200000;      ///< 地址为 4K 的整数
//                int actual_addr = (int) get_free_page();
                /**
                 * ptt 表赋值初始化，其中每个 pte 代表一个物理页的实际地址，即 actual_addr 为给进程分配的实际物理地址
                 * 00000000000000000000000000000_普通用户也可以访问_可读写_物理页有效（物理页不存在，访问会发生缺页中断）
                 */
                *item = 0b00000000000000000000000000000111 | actual_addr;
            }
        }
        else
        {
//            for (int j = 0; j < 0x400; ++j)
//            {
//                int* item = &ptt_arr[j];
//
//                int virtual_addr = j * 0x1000;
//                virtual_addr = virtual_addr + i * 0x400 * 0x1000;   ///< 0x400 * 0x1000 是跳过 i==0 时分配的地址。
//
//                /// 00000000000000000000000000000_普通用户也可以访问_可读写_物理页有效
//                *item = 0b00000000000000000000000000000111 | virtual_addr;
//            }
        }
    }

    BOCHS_DEBUG_MAGIC
    BOCHS_DEBUG_MAGIC

    set_cr3((uint)pdt);
    enable_page();

    BOCHS_DEBUG_MAGIC
    BOCHS_DEBUG_MAGIC
    return pdt;
}