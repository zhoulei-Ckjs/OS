#include "linux/tty.h"
#include "linux/kernel.h"

static char buf[1024];
/// TODO 无法打印 long long 类型
/// TODO 无法打印 '\t'
/**
 * 依据压栈规则，栈中数据如下：
 * 如 printk("%s", "123456")         "123456" 是定义了字符串常量，并返回地址
 * 高地址          stack                        指向的数据
 *  |       |       p       |       ->          "123456"        // 存储的是指针，指针指向 “123456” 的常量
 *  |       |      fmt      |       ->          "%s"            // 存储的是指针，指向字符串常量 "%s"
 * \/       |    返回地址    |
 * 低地址
 */
int printk(const char* format, ...)
{
    va_list p = 0;
    int i = 0;

    va_start(p, format);

    i = vsprintf(buf, format, p);

    va_end(p);

    console_write(buf, i);
    return i;
}