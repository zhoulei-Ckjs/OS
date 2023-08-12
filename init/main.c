#include "linux/tty.h"
#include "linux/kernel.h"

void main(void)
{
    /// 控制台初始化
    console_init();

    int i = 0;
    while(1)
    {
        printk("%d\n", i++);
    }
}