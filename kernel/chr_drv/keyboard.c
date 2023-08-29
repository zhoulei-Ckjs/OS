#include "linux/kernel.h"
#include "asm/io.h"

void keymap_handler(int idt_index)
{
    char ch = in_byte(0x60);
    printk("%c\n", ch);
}