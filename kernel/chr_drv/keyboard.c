#include "linux/kernel.h"

void keymap_handler(int idt_index)
{
    char ch = '0';
    printk("%c\n", ch);
}