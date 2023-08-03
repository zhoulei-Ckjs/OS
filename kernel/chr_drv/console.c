#include "sys/types.h"

#define VGA_TEXT_MODE_BUFFER_BASE 0xB8000
#define VGA_TEXT_MODE_BUFFER_LEN 0x4000
#define VGA_TEXT_MODE_BUFFER_END (VGA_TEXT_MODE_BUFFER_BASE + VGA_TEXT_MODE_BUFFER_LEN)

static uint screen;
static uint pos;
static uint x, y;

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
    while(ptr < VGA_TEXT_MODE_BUFFER_END)
        *ptr++ = 0x0720;
}

void console_init()
{
    console_clear();        ///< 清屏
}