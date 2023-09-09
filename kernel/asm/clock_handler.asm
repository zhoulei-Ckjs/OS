extern printk
global clock_handler_entry
clock_handler_entry:
    push msg
    call printk
    add esp, 4

    iret

msg:
    db "clock_handler", 10, 0