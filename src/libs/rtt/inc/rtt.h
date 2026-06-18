#ifndef __RTT_H__
#define __RTT_H__

#include <rtthread.h>

/**
 * rtt_write() - 向上行通道写入数据（目标→主机）
 * @data:	数据指针
 * @len:	请求写入长度
 *
 * 返回值：实际写入的字节数（0 表示缓冲区满），负值表示错误。
 */
rt_err_t rtt_write(const void* data, rt_size_t len);

/**
 * rtt_read() - 从下行通道读取数据（主机→目标）
 * @buf:	接收缓冲区
 * @len:	请求读取长度
 *
 * 返回值：实际读取的字节数（0 表示无数据），负值表示错误。
 */
rt_err_t rtt_read(void* buf, rt_size_t len);

#endif /* __RTT_H__ */