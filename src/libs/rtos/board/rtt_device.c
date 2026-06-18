/*
 * Copyright (c) 2025, PD-Embedded Project
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * RTT RT-Thread 字符设备驱动
 *
 * 将 Segger RTT 封装为标准 rt_device，使 Finsh/MSH 可直接通过 RTT
 * 进行控制台输入输出，无需串口硬件。
 *
 * ============================ 设计说明 ============================
 *
 * 写路径（目标→主机）：
 *   _rtt_dev_write() → rtt_write() → RTT 上行缓冲区 → 主机 RTT Viewer
 *   直接写入即可，无需额外机制。
 *
 * 读路径（主机→目标 → Finsh）：
 *   Finsh 的 finsh_getchar() 工作方式如下：
 *     (1) 调用 rt_device_read(device, -1, &ch, 1)
 *     (2) 若返回 != 1（无数据），永久阻塞在 rx_sem 上
 *     (3) rx_sem 仅由驱动的 rx_indicate 回调释放
 *
 *   串口驱动可以通过硬件 RX 中断触发 rx_indicate，但 RTT 下行通道
 *   （主机→目标）没有中断机制 —— 主机通过调试器 DAP 直接写入目标 RAM
 *   中的环形缓冲区，目标 CPU 无法感知写入事件。
 *
 *   本驱动采用"驱动内部轮询"策略：
 *     _rtt_dev_read() 在发现无数据时，不立即返回 0（否则 Finsh 将
 *     永久阻塞在不会到来的 rx_sem 上），而是以 rt_thread_delay(1)
 *     周期轮询下行缓冲区，直到数据到达。每次延迟让出 CPU，不影响
 *     其他线程运行。
 *
 *   这样既无需外部定时器，也无需 rx_indicate 回调，代码更简洁，
 *   且延迟更低（以 RT_TICK_PER_SECOND=1000 为例，最坏延迟 1ms，
 *   优于定时器方案的 20ms）。
 *
 *   注意：这是 RTT 无中断特性的本质约束，与串口中断方案只是"中断源"
 *   不同 —— 串口用硬件中断，RTT 用线程上下文轮询。
 * ================================================================
 */

#include <rthw.h>
#include <rtt.h>
#include <rtthread.h>

/* ================================================================== */
/*  设备对象                                                           */
/* ================================================================== */

static struct rt_device _rtt_device;

/* ================================================================== */
/*  设备操作函数                                                        */
/* ================================================================== */

static rt_err_t _rtt_dev_init(rt_device_t dev)
{
        (void)dev;
        return RT_EOK;
}

static rt_err_t _rtt_dev_open(rt_device_t dev, rt_uint16_t oflag)
{
        (void)dev;
        (void)oflag;
        return RT_EOK;
}

static rt_err_t _rtt_dev_close(rt_device_t dev)
{
        (void)dev;
        return RT_EOK;
}

static rt_ssize_t _rtt_dev_read(rt_device_t dev, rt_off_t pos, void *buffer,
                                rt_size_t size)
{
        int ret;

        (void)dev;
        (void)pos;

        /*
         * 先尝试一次非阻塞读取 —— 如果主机已经有数据写入下行通道，
         * 直接返回，不走轮询路径。
         */
        ret = rtt_read(buffer, size);
        if (ret > 0)
                return (rt_ssize_t)ret;

        /*
         * 下行通道暂无数据 → 驱动内部轮询等待。
         *
         * 为什么不能返回 0？
         *   Finsh 的 finsh_getchar() 在 rt_device_read() 返回值 != 1
         *   时会永久阻塞在 rx_sem 上（RT_WAITING_FOREVER），而 RTT 没
         *   有任何中断能触发 rx_indicate 释放该信号量。返回 0 将导致
         *   Finsh 永远休眠，控制台失去响应。
         *
         * 为什么用 rt_thread_mdelay(10) 轮询？
         *   每轮让出 CPU 10ms 后再试，既不会空转占满 CPU，又能在
         *   数据到达后迅速返回（最坏等待 10ms）。以一个 tick = 1ms
         *   为例，延迟远优于外部定时器方案（通常 20~50ms）。
         *
         * 这会影响其他线程吗？
         *   不会。Finsh 线程每次调用 rt_thread_mdelay(10) 后进入挂起
         *   状态，调度器会切换到其他就绪线程。当 tick 中断到来时，
         *   Finsh 变为就绪，在优先级合适时被调度执行。
         */
        for (;;) {
                rt_thread_mdelay(10);
                ret = rtt_read(buffer, size);
                if (ret > 0)
                        return (rt_ssize_t)ret;
        }
}

static rt_ssize_t _rtt_dev_write(rt_device_t dev, rt_off_t pos,
                                 const void *buffer, rt_size_t size)
{
        int ret;

        (void)dev;
        (void)pos;

        ret = rtt_write(buffer, size);
        return (ret > 0) ? (rt_ssize_t)ret : 0;
}

static rt_err_t _rtt_dev_control(rt_device_t dev, int cmd, void *args)
{
        (void)dev;
        (void)cmd;
        (void)args;
        return RT_EOK;
}

/* ================================================================== */
/*  设备注册（板级初始化阶段）                                            */
/* ================================================================== */

static int _rtt_device_init(void)
{
        _rtt_device.type          = RT_Device_Class_Char;
        _rtt_device.init          = _rtt_dev_init;
        _rtt_device.open          = _rtt_dev_open;
        _rtt_device.close         = _rtt_dev_close;
        _rtt_device.read          = _rtt_dev_read;
        _rtt_device.write         = _rtt_dev_write;
        _rtt_device.control       = _rtt_dev_control;
        _rtt_device.rx_indicate   = RT_NULL;
        _rtt_device.tx_complete   = RT_NULL;
        _rtt_device.user_data     = RT_NULL;

        rt_device_register(&_rtt_device, "rtt",
                           RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_STREAM);

        return RT_EOK;
}
/* 在 rt_components_board_init() 中执行，早于 rt_console_set_device() */
INIT_BOARD_EXPORT(_rtt_device_init);