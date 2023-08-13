#include "sys/types.h"
#include "asm/io.h"
#include "linux/kernel.h"

#define VGA_TEXT_MODE_BUFFER_BASE 0xB8000
#define VGA_TEXT_MODE_BUFFER_LEN 0x7FFF                     ///< 从 0xB8000 到 0xBFFFF
#define VGA_TEXT_MODE_BUFFER_END (VGA_TEXT_MODE_BUFFER_BASE + VGA_TEXT_MODE_BUFFER_LEN)
#define VGA_TEXT_MODE_SCREEN_WIDTH 80                       ///< 屏幕文本列数，每行显示80个字节
#define VGA_TEXT_MODE_SCREEN_HEIGHT 25                      ///< 屏幕文本行数
#define VGA_TEXT_MODE_SCREEN_ROW_SIZE (VGA_TEXT_MODE_SCREEN_WIDTH * 2)    ///< 每行字节数，每行显示80个字节（但是每个字节后面都有一个表示颜色的字节），所以就是160字节
#define VGA_TEXT_MODE_SCREEN_SCR_SIZE (VGA_TEXT_MODE_SCREEN_ROW_SIZE * VGA_TEXT_MODE_SCREEN_HEIGHT)    ///< 屏幕字节数

/**
 * 索引寄存器，一般用法：
 *  1. 往 0x3D4 发送一个值（这个值是协议规定，你要做什么操作就发送什么值）
 *  2. 有相应操作了，就可以操作 0x3D5 了，如果是读取数据就从 0x3D5 读取数据，是写入就往 0x3D5 写数据
 */
#define CRT_ADDR_REG 0x3D4      ///< CRT(6845)索引寄存器
#define CRT_DATA_REG 0x3D5      ///< CRT(6845)索引寄存器

/**
 * 根据 CRT 芯片的规格和文档
 * 向 0x3D4 发送 0xC 表示要操作显示起始位置的高位
 * 往 0x3D4 发送 0xD 表示要操作显示起始位置的低位
 */
#define CRT_START_ADDR_H 0xC    ///< 显示内存起始位置 - 高位
#define CRT_START_ADDR_L 0xD    ///< 显示内存起始位置 - 低位

/**
 * 根据 CRT 芯片的规格和文档：
 *      往 0x3D4 发送 0xE 表示要操作光标高位
 *      往 0x3D4 发送 0xF 表示要操作光标低位
 */
#define CRT_CURSOR_H 0xE        ///< 光标位置 - 高位
#define CRT_CURSOR_L 0xF        ///< 光标位置 - 低位

#define ASCII_NUL 0x00          ///< 结尾符 \0
#define ASCII_BEL 0x07          ///< \a，警报提示音
#define ASCII_BS 0x08           ///< \b，退格符backspace
#define ASCII_HT 0x09           ///< \t，tab键
#define ASCII_LF 0x0A           ///< LINE FEED 换行符 \n
#define ASCII_VT 0x0B           ///< \v 垂直制表位
#define ASCII_FF 0x0C           ///< \f 滚动到下一行，并不定位到行首
#define ASCII_CR 0x0D           ///< \r 回车符
#define ASCII_DEL 0x7F          ///< delete 键，这个键 ascii 也是 a.out 的起始字符

static uint screen;             ///< 当前显示器开始的内存位置（屏幕最最上角的位置）
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
 * @brief 设置显示器显示的起始位置
 *
 * 0xb8000 到0 xc000 都是显存的位置，让显示器从固定的内存位置显示开始显示
 */
static void set_screen()
{
    out_byte(CRT_ADDR_REG, CRT_START_ADDR_H);       ///< 要操作显示起始位置的高位
    /// 当前位置 - 0xb8000（首地址）右移 9 位
    /// 右移 9 位是因为低位占 8 位，而屏幕上的每一个字符都占 2 位字节，控制器是以字符为单位的，即右移 8 位再右移 1 位，即右移 9 位
    out_byte(CRT_DATA_REG, ((screen - VGA_TEXT_MODE_BUFFER_BASE) >> 9) & 0xFF);     ///< 向显示起始位置的高位写数据
    out_byte(CRT_ADDR_REG, CRT_START_ADDR_L);       ///< 要操作显示起始位置的低位
    /// 屏幕上的没一个字符占 2 个字节，控制器是以字符为单位的，所有我们要右移 1 位
    out_byte(CRT_DATA_REG, ((screen - VGA_TEXT_MODE_BUFFER_BASE) >> 1) & 0xFF);     ///< 向显示起始位置的低位写数据
}

/**
 * @brief 清屏，实际上是往 0xB8000 的内存位置写空格，写满就是清屏
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

/**
 * @brief '\b' 退格符 backspace 的处理
 */
static void command_bs()
{
    if(x > 0)
    {
        x--;
        pos -= 2;
        *((u16*)pos) = 0x0720;
    }
}

/**
 * @brief 向上滚屏
 */
static void scroll_up()
{
    /// 没有达到全部映射的内存
    if(screen + VGA_TEXT_MODE_SCREEN_SCR_SIZE + VGA_TEXT_MODE_SCREEN_ROW_SIZE < VGA_TEXT_MODE_BUFFER_END)
    {
        unsigned short *ptr = screen + VGA_TEXT_MODE_SCREEN_SCR_SIZE;
        for(size_t i = 0; i < VGA_TEXT_MODE_SCREEN_WIDTH; i++)
        {
            *ptr++ = 0x0720;
        }
        screen += VGA_TEXT_MODE_SCREEN_ROW_SIZE;    ///< 显示的起始位置 + 一行
        pos += VGA_TEXT_MODE_SCREEN_ROW_SIZE;       ///<
    }
    else    ///< 已经达到映射的内存了，那么可能要循环到开始再显示了
    {
        memcpy(VGA_TEXT_MODE_BUFFER_BASE, screen + VGA_TEXT_MODE_SCREEN_ROW_SIZE, VGA_TEXT_MODE_SCREEN_SCR_SIZE);
        pos -= (screen - VGA_TEXT_MODE_BUFFER_BASE);    ///< 更新光标位置。
        screen = VGA_TEXT_MODE_BUFFER_BASE;             ///< 更新当前屏幕开始位置。
        unsigned short *ptr = (screen + VGA_TEXT_MODE_SCREEN_SCR_SIZE - VGA_TEXT_MODE_SCREEN_ROW_SIZE);
        for(int i = 0; i < VGA_TEXT_MODE_SCREEN_WIDTH; i++)
        {
            *ptr++ = 0x0720;
        }
    }
    set_screen();
}

/**
 * @brief 换行的命令处理，即 ASCII 为 0A，即'\n'的处理
 */
static void command_lf()
{
    if(y + 1 < VGA_TEXT_MODE_SCREEN_HEIGHT)
    {
        y++;
        pos += VGA_TEXT_MODE_SCREEN_ROW_SIZE;
        return;
    }
    scroll_up();                        ///< 滚屏
}

/**
 * @brief 让程序定位到行首
 */
static void command_cr()
{
    pos -= (x << 1);
    x = 0;
}

/**
 * @brief delete 键，没有特殊的转义字符，ascii 码为 0x7F
 */
static void command_del()
{
    *((unsigned short*)(pos)) = 0x0720; ///< 将当前位置字符置空
}

void console_write(char* buf, unsigned int count)
{
    char ch = '\0';
    char *ptr = NULL;
    while(count--)
    {
        ch = *buf++;
        switch (ch)
        {
            /// '\0'的处理 不打印
            case ASCII_NUL:
                break;
            /// '\a'警报提示音，暂不处理
            case ASCII_BEL:
                /// TODO
                break;
            /// '\b'退格符，backspace
            case ASCII_BS:
                command_bs();
                break;
            /// '\t' tab键，暂不处理
            case ASCII_HT:
                break;
            /// '\n' 的处理
            case ASCII_LF:
                command_lf();           ///< 换行
                command_cr();           ///< 回车
                break;
            /// \v' 垂直制表位 暂不处理
            case ASCII_VT:
                break;
            /// '\f' 滚动一行（只换行，不定位到行首）
            case ASCII_FF:
                command_lf();
                break;
            /// '\r' 回车符的处理
            case ASCII_CR:
                command_cr();
                break;
            /// 'del' 键  ascii = 0x7f
            case ASCII_DEL:
                command_del();
                break;
            default:
                if(x >= VGA_TEXT_MODE_SCREEN_WIDTH)
                {
                    x = 0;
                    pos -= VGA_TEXT_MODE_SCREEN_ROW_SIZE;
                    command_lf();
                }
                ptr = (char*)pos;       ///< 更改指针位置
                *ptr = ch;              ///< 写一个字符
                *(ptr + 1) = 0x07;      ///< 黑底白字
                pos += 2;               ///< 当前位置更新
                x++;
        }
    }
    set_cursor();                       ///< 设置光标位置
}