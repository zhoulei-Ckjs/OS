#include "linux/traps.h"
#include "asm/io.h"

#define PIC_M_CTRL  0x20            ///< 主片的控制端口
#define PIC_M_DATA  0x21            ///< 主片的数据端口
#define PIC_S_CTRL  0xa0            ///< 从片的控制端口
#define PIC_S_DATA  0xa1            ///< 从片的数据端口
#define PIC_EOI     0x20            ///< 通知中断控制器中断结束

void send_eoi(int idt_index)
{
    /**
     * 8259A芯片，用于管理硬件中断。
     * 通常有两个 PIC：主 PIC (Master) 和从 PIC (Slave)
     * 主 PIC 处理 IRQ0-IRQ7，从 PIC 处理 IRQ8-IRQ15
     */

    /// 0x20-0x27：主 PIC 的中断 (IRQ0-IRQ7)
    if (idt_index >= 0x20 && idt_index <= 0x27)
    {
        out_byte(PIC_M_CTRL, PIC_EOI);
    }
    /// 0x28-0x2F：从 PIC 的中断 (IRQ8-IRQ15)
    /// 如果 idt_index 在从 PIC 管理的 IRQ 0x28 到 IRQ 0x2F 范围内，需要分别向主 PIC 和从 PIC 发送 EOI 信号。
    /// 这是因为从 PIC 中断也会触发主 PIC 中断。
    else if (idt_index >= 0x28 && idt_index <= 0x2f)
    {
        /// 因为从 PIC 连接到主 PIC 的 IRQ2 上，当中断来自从 PIC 时，需要同时通知主 PIC 和从 PIC 中断已处理完成。
        out_byte(PIC_M_CTRL, PIC_EOI);
        out_byte(PIC_S_CTRL, PIC_EOI);
    }
}

void write_xdt_ptr(xdt_ptr_t* p, int base, short limit)
{
    p->limit = limit;
    p->base = base;
}