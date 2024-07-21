#include "../include/linux/mm.h"
#include "../include/asm/system.h"
#include "../include/linux/kernel.h"
#include "../include/string.h"

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
            /// 每个进程的内存分为: \f 进程内存 = 内核内存 + 进程自由使用内存 \f
            ///
            /// 因为一个进程要使用内核的系统调用等，所以我们需要将内核内存映射给进程。
            /// 当前我们的内核放到了 BIOS 的存储空间中，而 BIOS 为 1M 的大小
            /// 所以内核部分，我们只需要映射 1M 大小即可。
            /// \f 1M = 1024 × 1024B \f
            ///
            /// 一个 pde 有 4096B，那么 1M 空间需要 pde 数量：
            /// \f 1024 × 1024 / 4096 = 256 = 0x100 \f
            for (int j = 0; j < 0x102; ++j)
            {
                int* item = &ptt_arr[j];

                int actual_addr = j * 0x1000;      ///< 地址为 4K 的整数
                /**
                 * ptt 表赋值初始化，其中每个 pte 代表一个物理页的实际地址，即 actual_addr 为给进程分配的实际物理地址
                 * 00000000000000000000000000000_普通用户也可以访问_可读写_物理页有效（物理页不存在，访问会发生缺页中断）
                 */
                *item = 0b00000000000000000000000000000111 | actual_addr;
            }
            /**
             * 我们映射页表要映射完全，一个 pdt 表有 pde（1 个 pde 4B）的个数为：
             * \f 4096B / 4B = 1024 = 0x400 \f
             *
             * 我们的页表 map 管理放在了物理内存的 0x100000-0x102000的位置
             */
            for (int j = 0x102; j < 0x400; ++j)
            {
                int* item = &ptt_arr[j];

                int actual_addr = (int) get_free_page();     ///< 地址为 4K 的整数
                *item = 0b00000000000000000000000000000111 | actual_addr;
            }
        }
        else
        {
            for (int j = 0; j < 0x400; ++j)
            {
                int* item = &ptt_arr[j];

                int virtual_addr = (int) get_free_page();

                /// 00000000000000000000000000000_普通用户也可以访问_可读写_物理页有效
                *item = 0b00000000000000000000000000000111 | virtual_addr;
            }
        }
    }

    BOCHS_DEBUG_MAGIC

    set_cr3((uint)pdt);
    enable_page();

    BOCHS_DEBUG_MAGIC
    return pdt;
}