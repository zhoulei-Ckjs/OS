extern printk
extern clock_handler

global clock_handler_entry
clock_handler_entry:
    push 0x20
    call clock_handler
    add esp, 4

    iret