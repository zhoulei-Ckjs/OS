#include "linux/tty.h"

int printk(const char* format, ...)
{
    console_write("123456", 6);

    return 0;
}