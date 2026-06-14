/*
 * Copyright (C) 2025, Reginald
 *
 * rrt.h - Real-Time Transfer 单通道上下传输模块
 *
 * 高内聚低耦合的 RTT 实现，Linux 编码风格。
 * 提供单 upstream (目标→主机) 和单 downstream (主机→目标) 通道。
 */

#ifndef __RTT_H__
#define __RTT_H__

#include <stddef.h>
#include <stdint.h>

/* ------------------------------------------------------------------ */
/*  常量定义                                                           */
/* ------------------------------------------------------------------ */

#define RTT_MAGIC 0x52545400U /* "RTT\0" - 用于调试器识别 CB */

/* 通道方向 */
#define RTT_UP 0   /* 上行：目标→主机 */
#define RTT_DOWN 1 /* 下行：主机→目标 */

/* API 返回值 */
#define RTT_ERR_NONE 0
#define RTT_ERR_BUSY -1
#define RTT_ERR_INVAL -2
#define RTT_ERR_NOMEM -3

/* ------------------------------------------------------------------ */
/*  通道描述符                                                         */
/* ------------------------------------------------------------------ */

/**
 * struct rtt_channel - 单通道描述符
 * @buf:	环形缓冲区基址
 * @size:	缓冲区大小（必须为 2 的幂）
 * @wr_off:	生产者写入偏移
 * @rd_off:	消费者读取偏移
 *
 * 上行通道：目标为生产者，主机为消费者。
 * 下行通道：主机为生产者，目标为消费者。
 *
 * 采用环形缓冲，通过 (write_offset - read_offset) & (size - 1)
 * 计算有效数据长度。
 */
struct rtt_channel
{
        const char* name;
        uint8_t* buffer;
        uint32_t size;
        volatile uint32_t write_offset;
        volatile uint32_t read_offset;
        uint32_t flags;
};

/* ------------------------------------------------------------------ */
/*  控制块                                                             */
/* ------------------------------------------------------------------ */

/**
 * struct rtt_cb - RTT 控制块
 * @magic:	魔数，供调试器扫描识别
 * @flags:	保留标志
 * @up:		上行通道（目标→主机，用于日志输出）
 * @down:	下行通道（主机→目标，用于命令输入）
 *
 * 该结构必须放置于 RAM 中的固定地址（通过链接脚本或节区属性），
 * 使调试器（如 J-Link RTT Viewer）可以通过内存扫描找到它。
 */
struct rtt_cb
{
        char id[16];
        uint32_t up_num;
        uint32_t down_num;
        struct rtt_channel up;
        struct rtt_channel down;
};

/* ------------------------------------------------------------------ */
/*  API 函数                                                           */
/* ------------------------------------------------------------------ */

/**
 * rtt_write() - 向上行通道写入数据（目标→主机）
 * @data:	数据指针
 * @len:	请求写入长度
 *
 * 返回值：实际写入的字节数（0 表示缓冲区满），负值表示错误。
 */
int rtt_write(const void* data, size_t len);

/**
 * rtt_read() - 从下行通道读取数据（主机→目标）
 * @buf:	接收缓冲区
 * @len:	请求读取长度
 *
 * 返回值：实际读取的字节数（0 表示无数据），负值表示错误。
 */
int rtt_read(void* buf, size_t len);

/**
 * rtt_puts() - 向上行通道写入字符串（目标→主机）
 * @str:	以 NUL 结尾的字符串
 *
 * 返回值：写入的字符数（不含 NUL），负值表示错误。
 */
int rtt_puts(const char* str);

#endif /* __RTT_H__ */