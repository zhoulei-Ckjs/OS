#include "linux/kernel.h"
#include "linux/mm.h"
#include "string.h"

#define ARDS_TIMES_BUFFER 0x8200

/**
 * @brief 物理内存
 */
physics_memory_info_t g_physics_memory;

void print_check_memory_info()
{
    check_memory_info_t* p = (check_memory_info_t*)ARDS_TIMES_BUFFER;
    check_memmory_item_t* p_data = (check_memmory_item_t*)(ARDS_TIMES_BUFFER + 2);

    unsigned short times = p->times;

    printk("memory check result:\n");
    printk("    base_higt    base_low   length_high  length_low   type\n");

    for (int i = 0; i < times; ++i)
    {
        check_memmory_item_t* tmp = p_data + i;
        printk("%10x, %10x, %10x, %12x, %6d\n", tmp->base_addr_high, tmp->base_addr_low,
               tmp->length_high, tmp->length_low, tmp->type);
    }
}

void memory_init()
{
    unsigned short* times = (unsigned short*)ARDS_TIMES_BUFFER;     ///< 内存块个数
    check_memmory_item_t* data = (check_memmory_item_t*)(ARDS_TIMES_BUFFER + 2);    ///< 内个存块地址
    for (int i = 0; i < *times; ++i)
    {
        check_memmory_item_t* tmp = data + i;
        if(tmp->base_addr_low > 0 && tmp->type == ZONE_VALID)
        {
            g_physics_memory.addr_start_ = tmp->base_addr_low;
            g_physics_memory.valid_mem_size_ = tmp->length_low;
            g_physics_memory.addr_end_ = tmp->base_addr_low + tmp->length_low;
        }
    }

    /// 我们只使用 1M 以上的内存，检测出的结果应该只有一块有效内存在 1M 以上，所以我们判断
    /// 是不是起始位置为 1M 就可以了。
    if (0x100000 != g_physics_memory.addr_start_)
    {
        printk("[%s:%d] no valid physics memory\n", __FILE__, __LINE__);
        printk("addr_start = %x\n", g_physics_memory.addr_start_);
        printk("addr_end = %x\n", g_physics_memory.addr_end_);
        printk("size = %x\n", g_physics_memory.valid_mem_size_);
        return;
    }

    /// 物理内存初始化
    physical_memory_init();

    /// 虚拟内存初始化
    virtual_memory_init();
}

void* get_free_page()
{
    bool find = false;
    int i = 0;              ///< 从0开始找，防止遗漏
    for(; i < g_physics_memory.pages_total_; ++i)
    {
        if(0 == g_physics_memory.map_[i])
        {
            find = true;
            break;
        }
    }
    if(find == false)
    {
        printk("memory used up!");
        return NULL;
    }
    g_physics_memory.map_[i] = 1;
    g_physics_memory.pages_used_++;
    g_physics_memory.pages_free_--;

    void* ret = (void*)(g_physics_memory.addr_start_) + i * PAGE_SIZE;
    printk("[%s] return: 0x%X, used: %d/%d pages\n", __FUNCTION__, ret,
           g_physics_memory.pages_used_, g_physics_memory.pages_total_);
    return ret;
}

void free_page(void* page_start_addr)
{
    int addr = (int)page_start_addr;
    if(addr < g_physics_memory.addr_start_ || addr > g_physics_memory.addr_end_)
    {
        printk("invalid address: 0x%x!\n", page_start_addr);
        return;
    }
    int index = (addr - g_physics_memory.addr_start_) / PAGE_SIZE;
    g_physics_memory.map_[index] = 0;
    g_physics_memory.pages_used_--;
    g_physics_memory.pages_free_++;

    printk("[%s] freed: 0x%X, used: %d/%d pages\n", __FUNCTION__,
           page_start_addr, g_physics_memory.pages_used_, g_physics_memory.pages_total_);
}

void physical_memory_init()
{
    /// 初始化物理内存
    g_physics_memory.pages_total_ = g_physics_memory.valid_mem_size_ / PAGE_SIZE;     ///< 4096 byte 为 1 页
    g_physics_memory.pages_used_ = 0;
    g_physics_memory.pages_free_ = g_physics_memory.pages_total_ - g_physics_memory.pages_used_;

    /// 初始化物理页表
    g_physics_memory.map_ = (uchar*)(g_physics_memory.addr_start_);
    memset(g_physics_memory.map_, 0, g_physics_memory.pages_total_);
    uint bitmap_item_used_ = g_physics_memory.pages_total_ / PAGE_SIZE;      ///< 1B 映射一个 page，共需要这么多 page
    if (0 != g_physics_memory.pages_total_ % PAGE_SIZE)
    {
        bitmap_item_used_ += 1;
    }
    for (int i = 0; i < bitmap_item_used_; ++i)     ///< 页表标记为占用状态
    {
        g_physics_memory.map_[i] = 1;
        g_physics_memory.pages_used_++;
        g_physics_memory.pages_free_--;
    }
    printk("physical memory block 0x%x\n"
           "    this memory block starts at 0x%x, ends at 0x%x\n"
           "    it contains %d pages(each page %dB)\n"
           "    physical memory map starts at: 0x%X, using %d pages, %d pages left!\n",
           &g_physics_memory,
           g_physics_memory.addr_start_, g_physics_memory.addr_end_,
           g_physics_memory.pages_total_, PAGE_SIZE,
           g_physics_memory.map_, g_physics_memory.pages_used_, g_physics_memory.pages_free_);
}