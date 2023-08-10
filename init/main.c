#include "linux/tty.h"
#include "linux/kernel.h"

void main(void)
{
    /// 控制台初始化
    console_init();

    console_write("123\n", 4);
    printk("%d%s", 5, "_def");
}