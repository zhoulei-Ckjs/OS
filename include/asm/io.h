#ifndef OS_IO_H
#define OS_IO_H

/**
 * @brief 从一个端口接收一个字节数据
 * @param port 端口
 * @return 接收的字节
 */
char in_byte(int port);

/**
 * @brief 向指定端口发送数据
 * @param port 端口
 * @param value 要发送的数据
 */
void out_byte(int port, int value);

#endif //OS_IO_H
