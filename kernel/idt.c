#include "linux/kernel.h"
#include "linux/head.h"
#include "string.h"

void idt_init()
{
    printk("idt init...");
    interrupt_descriptor temp;
    memset(&temp, '\0', sizeof(temp));
}