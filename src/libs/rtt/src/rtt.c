#include <rtt_port.h>
#include <string.h>

/* 静态检查 */
_Static_assert((RTT_UP_BUF_SIZE & (RTT_UP_BUF_SIZE - 1)) == 0,
               "UP buffer size must be power of 2");
_Static_assert((RTT_DOWN_BUF_SIZE & (RTT_DOWN_BUF_SIZE - 1)) == 0,
               "DOWN buffer size must be power of 2");

#include <rtt.h>

/**
 * struct rtt_channel - 单通道描述符
 * @buf:	环形缓冲区基址
 * @size:	缓冲区大小（必须为 2 的幂）
 * @wr_off:     生产者写入偏移
 * @rd_off:	消费者读取偏移
 *
 * 采用环形缓冲，通过 (write_offset - read_offset) & (size - 1)
 * 计算有效数据长度。
 */
struct rtt_channel
{
        const char* name;
        volatile uint8_t* buffer;
        const rt_size_t size;
        volatile rt_size_t write_offset;
        volatile rt_size_t read_offset;
        const uint32_t flags;
};

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
        const char id[16];
        const rt_size_t up_num;
        const rt_size_t down_num;
        struct rtt_channel up;
        struct rtt_channel down;
};

/* 上行缓冲区 */
static volatile uint8_t _rtt_up_buf[RTT_UP_BUF_SIZE] RTT_BUF_SECTION;

/* 下行缓冲区 */
static volatile uint8_t _rtt_down_buf[RTT_DOWN_BUF_SIZE] RTT_BUF_SECTION;

/* 控制块 */
static struct rtt_cb _rtt_cb RTT_CB_SECTION = {
        .id = "SEGGER RTT",
        .up_num = 1,
        .down_num = 1,
        .up =
                {
                        .name = "finsh-up",
                        .buffer = _rtt_up_buf,
                        .size = RTT_UP_BUF_SIZE,
                        .write_offset = 0,
                        .read_offset = 0,
                        .flags = 1,
                },
        .down =
                {
                        .name = "finsh-down",
                        .buffer = _rtt_down_buf,
                        .size = RTT_DOWN_BUF_SIZE,
                        .write_offset = 0,
                        .read_offset = 0,
                        .flags = 1,
                },
};

/**
 * _rtt_write() - 内部：向上行通道写入数据（目标→主机）
 * @data:	数据指针
 * @len:	请求写入长度
 *
 * 注意：调用者必须已持有 RTT_LOCK。
 *
 * 偏移量在 [0, size) 范围内回绕，采用"空一格"策略区分满与空：
 *   有效数据 = (write_offset - read_offset) & (size - 1)
 *   可用空间 = size - 1 - 有效数据
 * 写入后 write_offset 回绕更新，并通过 DMB 保证调试器看到完整数据。
 *
 * 返回值：实际写入的字节数。
 */
static rt_err_t _rtt_write(const void* data, rt_size_t len)
{
        struct rtt_channel* const up = &_rtt_cb.up;
        const rt_size_t wr = up->write_offset;
        const rt_size_t rd = up->read_offset;
        const rt_size_t used = (wr - rd) & (up->size - 1u);
        const rt_size_t avail = up->size - 1u - used;

        if (avail == 0)
        {
                return 0;
        }

        if (len > avail)
        {
                len = avail;
        }

        /* 写位置即 wr（已在 [0, size) 范围内） */
        const rt_size_t remaining = up->size - wr;

        if (len <= remaining)
        {
                memcpy((void*)&up->buffer[wr], data, len);
        }
        else
        {
                memcpy((void*)&up->buffer[wr], data, remaining);
                memcpy((void*)&up->buffer[0], (const uint8_t*)data + remaining,
                       len - remaining);
        }

        /* 数据内存屏障：确保调试器通过 DAP 读到完整数据后再看到新的
         * write_offset */
        rtt_port_dmb();

        /* 回绕更新写入偏移 */
        up->write_offset = (wr + len) & (up->size - 1u);

        return (rt_err_t)len;
}

/**
 * _rtt_read() - 内部：从下行通道读取数据（主机→目标）
 * @buf:	接收缓冲区
 * @len:	请求读取长度
 *
 * 注意：调用者必须已持有 RTT_LOCK。
 *
 * 偏移量在 [0, size) 范围内回绕：
 *   有效数据 = (write_offset - read_offset) & (size - 1)
 * 读出后 read_offset 回绕更新，并通过 DMB 保证生产者看到可用空间。
 *
 * 返回值：实际读取的字节数。
 */
static rt_err_t _rtt_read(void* buf, rt_size_t len)
{
        struct rtt_channel* const down = &_rtt_cb.down;
        const rt_size_t wr = down->write_offset;
        const rt_size_t rd = down->read_offset;
        const rt_size_t used = (wr - rd) & (down->size - 1u);

        if (used == 0)
        {
                return 0;
        }

        if (len > used)
        {
                len = used;
        }

        /* 读位置即 rd（已在 [0, size) 范围内） */
        const rt_size_t remaining = down->size - rd;

        if (len <= remaining)
        {
                memcpy(buf, (void*)&down->buffer[rd], len);
        }
        else
        {
                memcpy(buf, (void*)&down->buffer[rd], remaining);
                memcpy((uint8_t*)buf + remaining, (void*)&down->buffer[0],
                       len - remaining);
        }

        /* 数据内存屏障：确保 read_offset 更新不会比数据读取先被调试器看到 */
        rtt_port_dmb();

        /* 回绕更新读取偏移 */
        down->read_offset = (rd + len) & (down->size - 1u);

        return (rt_err_t)len;
}

/**
 * rtt_write() - 向上行通道写入数据（目标→主机）
 * @data:	数据指针
 * @len:	请求写入长度
 *
 * 返回值：实际写入的字节数。
 *  -  0：缓冲区已满
 *  - <0：错误
 */
rt_err_t rtt_write(const void* data, rt_size_t len)
{
        if (data == NULL)
        {
                return RT_EINVAL;
        }
        if (len == 0)
        {
                return RT_EOK;
        }

        const rt_base_t level = rt_spin_lock_irqsave(&RTT_LOCK);
        const rt_base_t ret = _rtt_write(data, len);
        rt_spin_unlock_irqrestore(&RTT_LOCK, level);

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
rt_err_t rtt_read(void* buf, rt_size_t len)
{
        if (buf == NULL)
        {
                return -RT_EINVAL;
        }

        if (len == 0)
        {
                return RT_EOK;
        }

        const rt_base_t level = rt_spin_lock_irqsave(&RTT_LOCK);
        const rt_base_t ret = _rtt_read(buf, len);
        rt_spin_unlock_irqrestore(&RTT_LOCK, level);

        return ret;
}