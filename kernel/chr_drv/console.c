#include "sys/types.h"
#include "asm/io.h"

#define VGA_TEXT_MODE_BUFFER_BASE 0xB8000
#define VGA_TEXT_MODE_BUFFER_LEN 0x4000
#define VGA_TEXT_MODE_BUFFER_END (VGA_TEXT_MODE_BUFFER_BASE + VGA_TEXT_MODE_BUFFER_LEN)

/**
 * 索引寄存器，一般用法：
 *  1. 往 0x3D4 发送一个值（这个值是协议规定，你要做什么操作就发送什么值）
 *  2. 有相应操作了，就可以操作 0x3D5 了，如果是读取数据就从 0x3D5 读取数据，是写入就往 0x3D5 写数据
 */
#define CRT_ADDR_REG 0x3D4      ///< CRT(6845)索引寄存器
#define CRT_DATA_REG 0x3D5      ///< CRT(6845)索引寄存器

/**
 * 根据 CRT 芯片的规格和文档：
 *      往 0x3D4 发送 0xE 表示要操作光标高位
 *      往 0x3D4 发送 0xF 表示要操作光标低位
 */
#define CRT_CURSOR_H 0xE        ///< 光标位置 - 高位
#define CRT_CURSOR_L 0xF        ///< 光标位置 - 低位

static uint screen;             ///< 当前显示器开始的内存位置
static uint pos;                ///< 记录当前光标的内存位置
static uint x, y;               ///< 当前光标的坐标

/**
 * @brief 将光标设定为当前 pos 所指向的位置
 */
static void set_cursor()
{
    out_byte(CRT_ADDR_REG, CRT_CURSOR_H);           ///< 要操作光标高位
    out_byte(CRT_DATA_REG, ((pos - VGA_TEXT_MODE_BUFFER_BASE) >> 9) & 0xFF);    ///< 往光标高位写数据
    /// 当前位置 - 0xB8000（首地址）右移 9 位
    /// 右移 9 位是因为低位占 8 位，而屏幕上的每一个字符都占 2 字节，即右移 8 位再除以 2，即右移 9 位
    out_byte(CRT_ADDR_REG, CRT_CURSOR_L);           ///< 要操作光标低位
    out_byte(CRT_DATA_REG, ((pos - VGA_TEXT_MODE_BUFFER_BASE) >> 1) & 0xFF);    ///< 往光标低位写数据
}

/**
 * @brief 清屏
 */
void console_clear()
{
    screen = VGA_TEXT_MODE_BUFFER_BASE;
    pos = VGA_TEXT_MODE_BUFFER_BASE;
    x = 0;
    y = 0;
    u16* ptr = (u16*)VGA_TEXT_MODE_BUFFER_BASE;
    while(ptr < (u16*)VGA_TEXT_MODE_BUFFER_END)
        *ptr++ = 0x0720;

    set_cursor();           ///< 设置光标位置
}

void console_init()
{
    console_clear();        ///< 清屏
}