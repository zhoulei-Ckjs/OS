#include "linux/tty.h"
#include "linux/kernel.h"

void main(void)
{
    /// 控制台初始化
    console_init();
    /// 初始化 gdt 表
    gdt_init();
}