/*
 * Copyright (C) 2025, Reginald
 *
 * rrt.c - Real-Time Transfer 核心实现
 *
 * 实现单通道上下传输的 RTT 模块：
 *   - 上行：目标 CPU 写入，调试器（主机）读取
 *   - 下行：调试器（主机）写入，目标 CPU 读取
 *
 * 采用环形缓冲区 + 内存屏障保证数据一致性，
 * 中断保护防止目标侧并发访问。
 */

#include "rtt.h"
#include "rtt_port.h"

_Static_assert((RTT_UP_BUF_SIZE & (RTT_UP_BUF_SIZE - 1)) == 0,
               "UP buffer size must be power of 2");
_Static_assert((RTT_DOWN_BUF_SIZE & (RTT_DOWN_BUF_SIZE - 1)) == 0,
               "DOWN buffer size must be power of 2");

/* ------------------------------------------------------------------ */
/*  SEGGER RTT 兼容定义                                                 */
/* ------------------------------------------------------------------ */

#define SEGGER_RTT_MAX_NUM_UP_BUFFERS 1
#define SEGGER_RTT_MAX_NUM_DOWN_BUFFERS 1

/* 上行缓冲区 */
static uint8_t rtt_up_buf[RTT_UP_BUF_SIZE] RTT_BUF_SECTION;

/* 下行缓冲区 */
static uint8_t rtt_down_buf[RTT_DOWN_BUF_SIZE] RTT_BUF_SECTION;

/* 控制块 */
struct rtt_cb _SEGGER_RTT RTT_CB_SECTION = {
        .id = {'S', 'E', 'G', 'G', 'E', 'R', ' ', 'R', 'T', 'T', 0, 0, 0, 0, 0,
               0},
        .up_num = SEGGER_RTT_MAX_NUM_UP_BUFFERS,
        .down_num = SEGGER_RTT_MAX_NUM_DOWN_BUFFERS,
        .up =
                {
                        .name = "terminal",
                        .buffer = rtt_up_buf,
                        .size = RTT_UP_BUF_SIZE,
                        .write_offset = 0,
                        .read_offset = 0,
                        .flags = 1,
                },
        .down =
                {
                        .name = "terminal",
                        .buffer = rtt_down_buf,
                        .size = RTT_DOWN_BUF_SIZE,
                        .write_offset = 0,
                        .read_offset = 0,
                        .flags = 1,
                },
};

/* ------------------------------------------------------------------ */
/*  辅助内联函数                                                       */
/* ------------------------------------------------------------------ */

/**
 * rtt_ring_avail() - 环形缓冲区中有效数据长度
 * @c:	通道描述符
 *
 * 返回值：生产者已写入但消费者尚未读取的字节数。
 */
static inline uint32_t rtt_ring_avail(const struct rtt_channel* c)
{
        return (c->write_offset - c->read_offset) & (c->size - 1);
}

/**
 * rtt_ring_space() - 环形缓冲区剩余可用空间
 * @c:    通道描述符
 *
 * 返回值：生产者还能写入的字节数（保留一个字节区分空/满）。
 */
static inline uint32_t rtt_ring_space(const struct rtt_channel* c)
{
        return (c->size - 1) -
               ((c->write_offset - c->read_offset) & (c->size - 1));
}

/**
 * rtt_mask() - 对缓冲区大小取模（假设 size 为 2 的幂）
 * @val:	原始偏移值
 * @size:	缓冲区大小
 */
static inline uint32_t rtt_mask(uint32_t val, uint32_t size)
{
        return val & (size - 1);
}

/* ------------------------------------------------------------------ */
/*  内部核心函数                                                       */
/* ------------------------------------------------------------------ */

/**
 * __rtt_write() - 向通道写入数据（无锁版本）
 * @c:	通道描述符
 * @data:	数据指针
 * @len:	请求写入长度
 *
 * 调用者必须已进入临界区。
 * 返回值：实际写入的字节数。
 */
static int __rtt_write(struct rtt_channel* c, const void* data, size_t len)
{
        const uint8_t* src = data;
        uint32_t space, remain, wr;
        uint32_t cnt = 0;

        space = rtt_ring_space(c);
        if (space == 0)
                return 0;

        if (len > space)
                len = space;

        wr = rtt_mask(c->write_offset, c->size);
        remain = c->size - wr;

        if (len <= remain)
        {
                /* 一次拷贝即可完成 */
                for (cnt = 0; cnt < len; cnt++)
                        ((uint8_t*)c->buffer)[wr + cnt] = src[cnt];
        }
        else
        {
                /* 需要分两段拷贝（回绕） */
                for (cnt = 0; cnt < remain; cnt++)
                        ((uint8_t*)c->buffer)[wr + cnt] = src[cnt];
                for (cnt = 0; cnt < len - remain; cnt++)
                        ((uint8_t*)c->buffer)[cnt] = src[remain + cnt];
        }

        /*
         * 内存屏障：确保数据写入完成后再更新 wr_off。
         * 主机端通过 DAP 读取时，需看到一致的数据。
         */
        rtt_port_dmb();

        c->write_offset += len;

        return (int)len;
}

/**
 * __rtt_read() - 从通道读取数据（无锁版本）
 * @c:	通道描述符
 * @buf:	接收缓冲区
 * @len:	请求读取长度
 *
 * 调用者必须已进入临界区。
 * 返回值：实际读取的字节数。
 */
static int __rtt_read(struct rtt_channel* c, void* buf, size_t len)
{
        uint8_t* dst = buf;
        uint32_t avail, remain, rd;
        uint32_t cnt = 0;

        avail = rtt_ring_avail(c);
        if (avail == 0)
                return 0;

        if (len > avail)
                len = avail;

        rd = rtt_mask(c->read_offset, c->size);
        remain = c->size - rd;

        if (len <= remain)
        {
                for (cnt = 0; cnt < len; cnt++)
                        dst[cnt] = ((uint8_t*)c->buffer)[rd + cnt];
        }
        else
        {
                for (cnt = 0; cnt < remain; cnt++)
                        dst[cnt] = ((uint8_t*)c->buffer)[rd + cnt];
                for (cnt = 0; cnt < len - remain; cnt++)
                        dst[len - remain + cnt] = ((uint8_t*)c->buffer)[cnt];
        }

        /*
         * 内存屏障：确保数据读取完成后再更新 rd_off。
         */
        rtt_port_dmb();

        c->read_offset += len;

        return (int)len;
}

/* ------------------------------------------------------------------ */
/*  公共 API                                                           */
/* ------------------------------------------------------------------ */

/**
 * rtt_write() - 向上行通道写入数据（目标→主机）
 * @data:	数据指针
 * @len:	请求写入长度
 *
 * 返回值：实际写入的字节数。
 *  -  0：缓冲区已满
 *  - <0：错误
 */
int rtt_write(const void* data, size_t len)
{
        unsigned long flags;
        int ret;

        if (data == NULL || len == 0)
                return -RTT_ERR_INVAL;

        flags = rtt_port_irq_save();
        ret = __rtt_write(&_SEGGER_RTT.up, data, len);
        rtt_port_irq_restore(flags);

        return ret;
}

/**
 * rtt_read() - 从下行通道读取数据（主机→目标）
 * @buf:	接收缓冲区
 * @len:	请求读取长度
 *
 * 返回值：实际读取的字节数。
 *  -  0：无可用数据
 *  - <0：错误
 */
int rtt_read(void* buf, size_t len)
{
        unsigned long flags;
        int ret;

        if (buf == NULL || len == 0)
                return -RTT_ERR_INVAL;

        flags = rtt_port_irq_save();
        ret = __rtt_read(&_SEGGER_RTT.down, buf, len);
        rtt_port_irq_restore(flags);

        return ret;
}

/**
 * rtt_puts() - 向上行通道写入字符串
 * @str:	NUL 结尾的字符串
 *
 * 返回值：写入的字符数（不含 NUL），负值表示错误。
 */
int rtt_puts(const char* str)
{
        size_t len;

        if (str == NULL)
                return -RTT_ERR_INVAL;

        for (len = 0; str[len] != '\0'; len++)
                /* nothing */;

        return rtt_write(str, len);
}