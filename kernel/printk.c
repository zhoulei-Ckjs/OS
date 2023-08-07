#include "linux/tty.h"
#include "linux/kernel.h"

static char buf[1024];
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