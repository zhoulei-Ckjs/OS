#include "../../include/asm/io.h"
#include "../../include/linux/tty.h"
#include "../../include/string.h"
#include "../../include/asm/system.h"

/**
 * 索引寄存器，一般用法：
 *  1. 往 0x3D4 发送一个值（这个值是协议规定，你要做什么操作就发送什么值）
 *  2. 有相应操作了，就可以操作 0x3D5 了，如果是读取数据就从 0x3D5 读取数据，是写入就往 0x3D5 写数据
 */
#define CRT_ADDR_REG 0x3D4                          // CRT(6845)索引寄存器
#define CRT_DATA_REG 0x3D5                          // CRT(6845)数据寄存器

/**
 * 根据 CRT 芯片的规格和文档：
 *      往 0x3D4 发送 0xE 表示要操作光标高位
 *      往 0x3D4 发送 0xF 表示要操作光标低位
 */
#define CRT_CURSOR_H 0xE                            // 光标位置 - 高位
#define CRT_CURSOR_L 0xF                            // 光标位置 - 低位

/**
 * 根据 CRT 芯片的规格和文档：
 *      往 0x3D4 发送 0xC 表示要操作显示起始位置的高位
 *      往 0x3D4 发送 0xD 表示要操作显示起始位置的低位
 */
#define CRT_START_ADDR_H 0xC                        // 显示内存起始位置 - 高位
#define CRT_START_ADDR_L 0xD                        // 显示内存起始位置 - 低位

#define MEM_BASE 0xB8000                            // 显卡内存起始位置
#define MEM_SIZE 0x4000                             // 显卡内存大小
#define MEM_END (MEM_BASE + MEM_SIZE)               // 显卡内存结束位置
#define WIDTH 80                                    // 屏幕文本列数，每行显示80个字节
#define HEIGHT 25                                   // 屏幕文本行数
#define ROW_SIZE (WIDTH * 2)                        // 每行字节数，每行显示80个字节（但是每个字节后面都有一个表示颜色的字节），所以就是160字节
#define SCR_SIZE (ROW_SIZE * HEIGHT)                // 屏幕字节数

#define ASCII_NUL 0x00                              // 结尾符 \0
#define ASCII_BEL 0x07                              // \a，警报提示音
#define ASCII_BS 0x08                               // \b，退格符backspace
#define ASCII_HT 0x09                               // \t，tab键
#define ASCII_LF 0x0A                               // LINE FEED 换行符 \n
#define ASCII_VT 0x0B                               // \v 垂直制表位
#define ASCII_FF 0x0C                               // \f 滚动到下一行，并不定位到行首
#define ASCII_CR 0x0D                               // \r 回车符
#define ASCII_DEL 0x7F                              // delete 键，这个键 ascii 也是 a.out 的起始字符

static uint screen;                                 // 当前显示器开始的内存位置
static uint pos;                                    // 记录当前光标的内存位置
static uint x, y;                                   // 当前光标的坐标

/**
 * 将光标设定为当前 pos 所指向的位置
 */
static void set_cursor()
{
    out_byte(CRT_ADDR_REG, CRT_CURSOR_H);                           // 要操作光标高位
    out_byte(CRT_DATA_REG, ((pos - MEM_BASE) >> 9) & 0xff);         // 往光标高位写数据
    // 当前位置 - 0xb8000（首地址）右移 9 位
    // 右移 9 位是因为低位占8位，而屏幕上的每一个字符都占2位，即右移8位再除以2，即右移9位
    out_byte(CRT_ADDR_REG, CRT_CURSOR_L);                           // 要操作光标低位
    out_byte(CRT_DATA_REG, ((pos - MEM_BASE) >> 1) & 0xff);         // 往光标低位写数据
}

/**
 * 设置显示器显示的起始位置
 *      因为 [0xb8000，0xc0000) 都是显存的位置，怎么让显示器从固定的内存位置显示开始显示呢？
 *      这个函数完成此功能
 */
static void set_screen()
{
    out_byte(CRT_ADDR_REG, CRT_START_ADDR_H);                       // 要操作显示起始位置的高位
    out_byte(CRT_DATA_REG, ((screen - MEM_BASE) >> 9) & 0xff);      // 往要显示起始位置的高位写数据
    // 当前位置 - 0xb8000（首地址）右移 9 位
    // 右移 9 位是因为低位占8位，而屏幕上的每一个字符都占2位字节，控制器是以字符为单位的，即右移8位再右移 1 位，即右移9位
    out_byte(CRT_ADDR_REG, CRT_START_ADDR_L);                       // 要操作显示起始位置的低位
    out_byte(CRT_DATA_REG, ((screen - MEM_BASE) >> 1) & 0xff);      // 往要显示起始位置的低位写数据
    // 屏幕上的没一个字符占2个字节，控制器是以字符为单位的，所有我们要右移 1 位
}

/**
 * 清屏，实际上是往0xb8000的内存位置写空格，写满就是清屏
 */
void console_clear()
{
    screen = MEM_BASE;
    pos = MEM_BASE;
    x = y = 0;

    set_cursor();                   // 设置光标位置
    set_screen();

    u16 *ptr = (u16 *)MEM_BASE;
    while (ptr < MEM_END)
    {
        *ptr++ = 0x0720;
    }
}

void console_init(void)
{
    console_clear();
}

/**
 * 向上滚屏
 */
static void scroll_up()
{
    if (screen + SCR_SIZE + ROW_SIZE < MEM_END)     // 没有达到全部映射的内存
    {
        // 对下一行进行清空操作，因为整个映射的显存是循环使用的，因此下一行可能被写入了数据，所以要清空
        u32 *ptr = (u32 *)(screen + SCR_SIZE);
        for (size_t i = 0; i < WIDTH; i++)
        {
            *ptr++ = 0x0720;
        }
        screen += ROW_SIZE;                         // 显示的起始位置 + 一行
        pos += ROW_SIZE;
    }
    else                                            // 已经达到映射的内存了，那么可能要循环到开始再显示了
    {
        memcpy(MEM_BASE, screen + ROW_SIZE, SCR_SIZE);      // 因为要滚动一行，所以 screen 开始那一行就不要了
        pos -= (screen - MEM_BASE);

        screen = MEM_BASE;
        u32 *ptr = (u32 *)(screen + SCR_SIZE - ROW_SIZE);
        for (size_t i = 0; i < WIDTH; i++)                  // 最后一行清空
        {
            *ptr++ = 0x0720;
        }
    }
    set_screen();
}

/**
 * 换行的命令处理，即 ASCII 为 0A，即'\n'的处理
 */
static void command_lf()
{
    if(y + 1 < HEIGHT)
    {
        y++;
        pos += ROW_SIZE;
        return;
    }
    scroll_up();
}

/**
 * 让程序定位到行首
 */
static void command_cr()
{
    pos -= (x << 1);
    x = 0;
}

/**
 * '\b' 退格符 backspace 的处理
 */
static void command_bs()
{
    if (x > 0)
    {
        x--;
        pos -= 2;
        *(u16 *)pos = 0x0720;
    }
}

/**
 * delete 键，没有特殊的转义字符，ascii 码为 0x7F
 */
static void command_del()
{
    *(u16 *)pos = 0x0720;                       // 将当前位置字符置空，有待完善
}

void console_write(char *buf, u32 count)
{
    CLI
    char ch;
    char *ptr = NULL;
    while (count--)
    {
        ch = *buf++;
        switch (ch)
        {
            /** '\0'的处理 不打印 **/
            case ASCII_NUL:
                break;
            /** '\a'警报提示音，暂不处理 **/
            case ASCII_BEL:
                break;
            /** '\b'退格符，backspace **/
            case ASCII_BS:
                command_bs();
                break;
            /** '\t' tab键，暂不处理 **/
            case ASCII_HT:
                break;
            /** '\n' 的处理 **/
            case ASCII_LF:
                command_lf();                   // 换行
                command_cr();                   // 跳到行首
                break;
            /** '\v' 垂直制表位 暂不处理 **/
            case ASCII_VT:
                break;
            /** '\f' 滚动一行 **/
            case ASCII_FF:
                command_lf();
                break;
            /** '\r' 回车符的处理 **/
            case ASCII_CR:
                command_cr();
                break;
            /** 'del' 键  ascii = 0x7f **/
            case ASCII_DEL:
                command_del();
                break;
            default:
                if (x >= WIDTH)                 // 横坐标超过80，该换行了
                {
                    x -= WIDTH;
                    pos -= ROW_SIZE;
                    command_lf();
                }
                ptr = (char *)pos;              // 更改指针位置
                *ptr = ch;                      // 写一个字符
                *(ptr + 1) = 0x07;              // 黑底白字
                pos += 2;                       // 当前位置向后移动
                x++;
        }
    }
    set_cursor();                               //  将光标设定为当前 pos 所指向的位置
    STI
}
