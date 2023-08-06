/**
 * @brief 终端控制相关。
 */

#ifndef OS_TTY_H
#define OS_TTY_H

/**
 * @brief 控制台初始化
 */
void console_init();

/**
 * @brief 向屏幕输出
 * @param buf 待输出内容
 * @param count 输出长度
 */
void console_write(char* buf, unsigned int count);

#endif //OS_TTY_H
