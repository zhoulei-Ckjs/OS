#include "../../include/linux/kernel.h"
#include "../../include/asm/io.h"
#include "../../include/linux/types.h"
#include "../../include/linux/traps.h"

/**
 * 一个按键按下会触发中断两次
 * 按下返回的码叫通码 make code
 * 弹起返回的码叫断码 break code
 *
 * 断码 = 通码 | 0x80
 * a 的通码 ： 0001 1110
 * a 的断码 ： 1001 1110
 */

#define INV 0         ///< 不可见字符
#define CODE_PRINT_SCREEN_DOWN 0xB7

typedef enum {
    KEY_NONE,       ///< 0
    KEY_ESC,
    KEY_1,
    KEY_2,
    KEY_3,
    KEY_4,
    KEY_5,
    KEY_6,
    KEY_7,
    KEY_8,
    KEY_9,          ///< 10
    KEY_0,
    KEY_MINUS,
    KEY_EQUAL,
    KEY_BACKSPACE,
    KEY_TAB,
    KEY_Q,
    KEY_W,
    KEY_E,
    KEY_R,
    KEY_T,          ///< 20
    KEY_Y,
    KEY_U,
    KEY_I,
    KEY_O,
    KEY_P,
    KEY_BRACKET_L,
    KEY_BRACKET_R,
    KEY_ENTER,
    KEY_CTRL_L,
    KEY_A,          ///< 30
    KEY_S,
    KEY_D,
    KEY_F,
    KEY_G,
    KEY_H,
    KEY_J,
    KEY_K,
    KEY_L,
    KEY_SEMICOLON,
    KEY_QUOTE,      ///< 40
    KEY_BACKQUOTE,
    KEY_SHIFT_L,
    KEY_BACKSLASH,
    KEY_Z,
    KEY_X,
    KEY_C,
    KEY_V,
    KEY_B,
    KEY_N,
    KEY_M,          ///< 50
    KEY_COMMA,
    KEY_POINT,
    KEY_SLASH,      ///< 53
    KEY_SHIFT_R,
    KEY_STAR,
    KEY_ALT_L,
    KEY_SPACE,
    KEY_CAPSLOCK,   ///< 58
    KEY_F1,
    KEY_F2,         ///< 60
    KEY_F3,
    KEY_F4,
    KEY_F5,
    KEY_F6,
    KEY_F7,
    KEY_F8,
    KEY_F9,
    KEY_F10,
    KEY_NUMLOCK,
    KEY_SCRLOCK,    ///< 70
    KEY_PAD_7,
    KEY_PAD_8,
    KEY_PAD_9,
    KEY_PAD_MINUS,
    KEY_PAD_4,
    KEY_PAD_5,
    KEY_PAD_6,
    KEY_PAD_PLUS,
    KEY_PAD_1,
    KEY_PAD_2,      ///< 80
    KEY_PAD_3,
    KEY_PAD_0,
    KEY_PAD_POINT,
    KEY_54,
    KEY_55,
    KEY_56,
    KEY_F11,
    KEY_F12,
    KEY_59,
    KEY_WIN_L,      ///< 90
    KEY_WIN_R,
    KEY_CLIPBOARD,
    KEY_5D,
    KEY_5E,

    KEY_PRINT_SCREEN,   ///< 95
} key_index_t;

static char keymap[][4] =
{
    /// 扫描码未与 shift 组合____与 shift 组合____普通码（如左侧ctrl键）____扩展码（如右ctrl键）
    /// 扫描码未与 shift 组合____与 shift 组合____（键按下为 true，否则 false）____扩展码（如右ctrl键）
    /* 0x00 */ {INV, INV, false, false},   // NULL
    /* 0x01 */ {0x1b, 0x1b, false, false}, // ESC
    /* 0x02 */ {'1', '!', false, false},
    /* 0x03 */ {'2', '@', false, false},
    /* 0x04 */ {'3', '#', false, false},
    /* 0x05 */ {'4', '$', false, false},
    /* 0x06 */ {'5', '%', false, false},
    /* 0x07 */ {'6', '^', false, false},
    /* 0x08 */ {'7', '&', false, false},
    /* 0x09 */ {'8', '*', false, false},
    /* 0x0A */ {'9', '(', false, false},
    /* 0x0B */ {'0', ')', false, false},
    /* 0x0C */ {'-', '_', false, false},
    /* 0x0D */ {'=', '+', false, false},
    /* 0x0E */ {'\b', '\b', false, false}, // BACKSPACE
    /* 0x0F */ {'\t', '\t', false, false}, // TAB
    /* 0x10 */ {'q', 'Q', false, false},
    /* 0x11 */ {'w', 'W', false, false},
    /* 0x12 */ {'e', 'E', false, false},
    /* 0x13 */ {'r', 'R', false, false},
    /* 0x14 */ {'t', 'T', false, false},
    /* 0x15 */ {'y', 'Y', false, false},
    /* 0x16 */ {'u', 'U', false, false},
    /* 0x17 */ {'i', 'I', false, false},
    /* 0x18 */ {'o', 'O', false, false},
    /* 0x19 */ {'p', 'P', false, false},
    /* 0x1A */ {'[', '{', false, false},
    /* 0x1B */ {']', '}', false, false},
    /* 0x1C */ {'\n', '\n', false, false}, // ENTER
    /* 0x1D */ {INV, INV, false, false},   // CTRL_L
    /* 0x1E */ {'a', 'A', false, false},
    /* 0x1F */ {'s', 'S', false, false},
    /* 0x20 */ {'d', 'D', false, false},
    /* 0x21 */ {'f', 'F', false, false},
    /* 0x22 */ {'g', 'G', false, false},
    /* 0x23 */ {'h', 'H', false, false},
    /* 0x24 */ {'j', 'J', false, false},
    /* 0x25 */ {'k', 'K', false, false},
    /* 0x26 */ {'l', 'L', false, false},
    /* 0x27 */ {';', ':', false, false},
    /* 0x28 */ {'\'', '"', false, false},
    /* 0x29 */ {'`', '~', false, false},
    /* 0x2A */ {INV, INV, false, false}, // SHIFT_L
    /* 0x2B */ {'\\', '|', false, false},
    /* 0x2C */ {'z', 'Z', false, false},
    /* 0x2D */ {'x', 'X', false, false},
    /* 0x2E */ {'c', 'C', false, false},
    /* 0x2F */ {'v', 'V', false, false},
    /* 0x30 */ {'b', 'B', false, false},
    /* 0x31 */ {'n', 'N', false, false},
    /* 0x32 */ {'m', 'M', false, false},
    /* 0x33 */ {',', '<', false, false},
    /* 0x34 */ {'.', '>', false, false},
    /* 0x35 */ {'/', '?', false, false},
    /* 0x36 */ {INV, INV, false, false},  // SHIFT_R
    /* 0x37 */ {'*', '*', false, false},  // PAD *
    /* 0x38 */ {INV, INV, false, false},  // ALT_L
    /* 0x39 */ {' ', ' ', false, false},  // SPACE
    /* 0x3A */ {INV, INV, false, false},  // CAPSLOCK
    /* 0x3B */ {INV, INV, false, false},  // F1
    /* 0x3C */ {INV, INV, false, false},  // F2
    /* 0x3D */ {INV, INV, false, false},  // F3
    /* 0x3E */ {INV, INV, false, false},  // F4
    /* 0x3F */ {INV, INV, false, false},  // F5
    /* 0x40 */ {INV, INV, false, false},  // F6
    /* 0x41 */ {INV, INV, false, false},  // F7
    /* 0x42 */ {INV, INV, false, false},  // F8
    /* 0x43 */ {INV, INV, false, false},  // F9
    /* 0x44 */ {INV, INV, false, false},  // F10
    /* 0x45 */ {INV, INV, false, false},  // NUMLOCK
    /* 0x46 */ {INV, INV, false, false},  // SCRLOCK
    /* 0x47 */ {'7', INV, false, false},  // pad 7 - Home
    /* 0x48 */ {'8', INV, false, false},  // pad 8 - Up
    /* 0x49 */ {'9', INV, false, false},  // pad 9 - PageUp
    /* 0x4A */ {'-', '-', false, false},  // pad -
    /* 0x4B */ {'4', INV, false, false},  // pad 4 - Left
    /* 0x4C */ {'5', INV, false, false},  // pad 5
    /* 0x4D */ {'6', INV, false, false},  // pad 6 - Right
    /* 0x4E */ {'+', '+', false, false},  // pad +
    /* 0x4F */ {'1', INV, false, false},  // pad 1 - End
    /* 0x50 */ {'2', INV, false, false},  // pad 2 - Down
    /* 0x51 */ {'3', INV, false, false},  // pad 3 - PageDown
    /* 0x52 */ {'0', INV, false, false},  // pad 0 - Insert
    /* 0x53 */ {'.', 0x7F, false, false}, // pad . - Delete
    /* 0x54 */ {INV, INV, false, false},  //
    /* 0x55 */ {INV, INV, false, false},  //
    /* 0x56 */ {INV, INV, false, false},  //
    /* 0x57 */ {INV, INV, false, false},  // F11
    /* 0x58 */ {INV, INV, false, false},  // F12
    /* 0x59 */ {INV, INV, false, false},  //
    /* 0x5A */ {INV, INV, false, false},  //
    /* 0x5B */ {INV, INV, false, false},  // Left Windows
    /* 0x5C */ {INV, INV, false, false},  // Right Windows
    /* 0x5D */ {INV, INV, false, false},  // Clipboard
    /* 0x5E */ {INV, INV, false, false},  //

    // Print Screen 是强制定义 本身是 0xB7
    /* 0x5F */ {INV, INV, false, false}, // PrintScreen
};

static bool capslock_state;                         ///< 大写锁定
static bool extcode_state;                          ///< 扩展码状态

/// SHIFT 键状态
#define shift_state (keymap[KEY_SHIFT_L][2] || keymap[KEY_SHIFT_R][2])

/// 键盘中断处理函数
void keymap_handler(int idt_index)
{
    uchar ext = 2;                                 ///< keymap 状态索引，默认没有 shift 键
    uchar scancode = in_byte(0x60);             ///< 0x60端口是用于接收键盘数据的输入端口

    /// 由于在 head.asm 中设置的为自动 EOI ，故这里不用发送 end of interrupt
    send_eoi(idt_index);                ///< 手动EOI

    /**
     * 扩展码用于标识与标准码不同的键。比如，右侧的 Ctrl 键对应的扫描码为 0xE0 0x1D，一个扩展码会有 4 个 byte
     * 普通码就 2 个
     * 而左侧的 Ctrl 键对应的扫描码为 0x1D。通过扩展码可以区分这两个键。
     */
    /// 是扩展码字节
    if (scancode == 0xe0)
    {
        /// 置扩展状态
        extcode_state = true;
        return;
    }

    /// 是扩展码
    if (extcode_state)
    {
        /// 改状态索引
        ext = 3;
        /// 修改扫描码，添加 0xe0 前缀
        scancode |= 0xe000;
        /// 扩展状态无效
        extcode_state = false;
    }

    ushort makecode = (scancode & 0x7f);            ///< 获得通码
    if (makecode == CODE_PRINT_SCREEN_DOWN)         ///< print screen 在有些键盘发送的是 0xe0 0x37 表示按下
    {
        makecode = KEY_PRINT_SCREEN;
    }

    /// 通码非法
    if (makecode > KEY_PRINT_SCREEN)
    {
        return;
    }

    /// 是否是断码，按键抬起
    bool breakcode = ((scancode & 0x0080) != 0);
    if (breakcode)                                  ///< 处理断码，按键抬起，直接返回
    {
        /// 如果是则设置状态
        keymap[makecode][ext] = false;
        return;
    }

    /// 下面是通码，按键按下
    keymap[makecode][ext] = true;

    /// 大写锁定，有的时候触发一次，再次按下后触发两次，不理解
    if (makecode == KEY_CAPSLOCK)
    {
        capslock_state = !capslock_state;
    }

    bool shift = false;                             ///< 大小写偏移
    if (capslock_state)
    {
        shift = !shift;
    }
    if (shift_state)                                ///< shift 键按下
    {
        shift = !shift;
    }

    /// 获得按键 ASCII 码
    char ch = 0;                                    ///< 这里一次按键会打印两次 a，通码一次，断码一次
    /// [/?] 这个键比较特殊，只有这个键扩展码和普通码一样，别的都有像 ctrl_l 和 ctrl_r 这样的区分
    if (ext == 3 && (makecode != KEY_SLASH))        ///< 如果是扩展按键（如 Ctrl_right）
    {
        ch = keymap[makecode][1];
    }
    else
        ch = keymap[makecode][shift];

    if (ch == INV)                                  ///< 如果是INV，则不打印
        return;
    printk("%c\n", ch);
}
