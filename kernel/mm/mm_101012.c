#include "linux/mm.h"
#include "string.h"
#include "asm/system.h"
#include "linux/kernel.h"

void virtual_memory_init()
{
    int *pdt = (int *) get_free_page();           ///< PDT表
    memset(pdt, 0, PAGE_SIZE);                    ///< 清零

    /// PDT 里面的每项，即 PDE，内容是 PTT 页表地址 + 尾 12 位的权限位。
    int ptt = (int) get_free_page();
    memset((int*)ptt, 0, PAGE_SIZE);
    /// 00000000000000000000000000000_普通用户也可以访问_可读写_物理页有效
    int pde = 0b00000000000000000000000000000111 | ptt;
    pdt[0] = pde;

    int *ptt_arr = (int *) ptt;

    /// 每个进程的内存分为: \f 进程内存 = 内核内存 + 进程自由使用内存 \f
    ///
    /// 因为一个进程要使用内核的系统调用等，所以我们需要将内核内存映射给进程。
    /// 当前我们的内核放到了 0x00 的存储空间中，大小不超过 1M，我们不妨映射 1M 大小，正好 1M 以下也不在我们的物理页表管理中。
    /// \f 1M = 1024 × 1024B \f
    ///
    /// 一个 pde 有 4096B，那么 1M 空间需要 pde 数量：
    /// \f 1024 × 1024 / 4096 = 256 = 0x100 \f
    ///
    /// 我们的物理页表 map 管理放在了物理内存的 0x100000 起始的位置，用了 2 页。
    for (int j = 0; j < 0x100; ++j)
    {
        int *item = &ptt_arr[j];

        int actual_addr = j * 0x1000;      ///< 地址为 4K 的整数
        /**
         * ptt 表赋值初始化，其中每个 pte 代表一个物理页的实际地址，即 actual_addr 为给进程分配的实际物理地址
         * 00000000000000000000000000000_普通用户也可以访问_可读写_物理页有效（物理页不存在，访问会发生缺页中断）
         */
        *item = 0b00000000000000000000000000000111 | actual_addr;
    }

    BOCHS_DEBUG_MAGIC
    set_cr3((uint)pdt);
    enable_page();
    BOCHS_DEBUG_MAGIC
}