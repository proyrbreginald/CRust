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
 * 输出路径：rtt_write() → 上行通道（目标→主机）
 * 输入路径：rtt_read()  → 下行通道（主机→目标）
 * 输入轮询：由 50 Hz 硬件定时器周期检查下行缓冲区，触发 rx_indicate
 *           回调以唤醒 Finsh 读取线程。
 */

#include <rthw.h>
#include <rtt.h>
#include <rtthread.h>

/* ================================================================== */
/*  设备对象 & 函数                                                     */
/* ================================================================== */

static struct rt_device _rtt_device;
static struct rt_timer _rtt_poll_timer;

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

static rt_ssize_t _rtt_dev_read(rt_device_t dev, rt_off_t pos, void* buffer,
                                rt_size_t size)
{
        int ret;

        (void)dev;
        (void)pos;

        ret = rtt_read(buffer, size);
        return (ret > 0) ? (rt_ssize_t)ret : 0;
}

static rt_ssize_t _rtt_dev_write(rt_device_t dev, rt_off_t pos,
                                 const void* buffer, rt_size_t size)
{
        int ret;

        (void)dev;
        (void)pos;

        ret = rtt_write(buffer, size);
        return (ret > 0) ? (rt_ssize_t)ret : 0;
}

static rt_err_t _rtt_dev_control(rt_device_t dev, int cmd, void* args)
{
        (void)dev;
        (void)cmd;
        (void)args;
        return RT_EOK;
}

/* ================================================================== */
/*  输入轮询（定时器回调）                                               */
/* ================================================================== */

static void _rtt_poll_entry(void* parameter)
{
        (void)parameter;

        /* 仅唤醒 Finsh，不读取数据。
         * 数据由 _rtt_dev_read() → rtt_read() 消费，避免重复读取导致数据丢失。 */
        if (_rtt_device.rx_indicate != RT_NULL)
                _rtt_device.rx_indicate(&_rtt_device, 0);
}

static int _rtt_poll_timer_start(void)
{
        rt_timer_init(&_rtt_poll_timer, "rttpol", _rtt_poll_entry, RT_NULL,
                      RT_TICK_PER_SECOND / 10, /* 100 ms */
                      RT_TIMER_FLAG_PERIODIC | RT_TIMER_FLAG_HARD_TIMER);
        rt_timer_start(&_rtt_poll_timer);

        return RT_EOK;
}
INIT_DEVICE_EXPORT(_rtt_poll_timer_start);

/* ================================================================== */
/*  设备注册（板级初始化阶段）                                            */
/* ================================================================== */

static int _rtt_device_init(void)
{
        _rtt_device.type = RT_Device_Class_Char;
        _rtt_device.init = _rtt_dev_init;
        _rtt_device.open = _rtt_dev_open;
        _rtt_device.close = _rtt_dev_close;
        _rtt_device.read = _rtt_dev_read;
        _rtt_device.write = _rtt_dev_write;
        _rtt_device.control = _rtt_dev_control;
        _rtt_device.rx_indicate = RT_NULL;
        _rtt_device.tx_complete = RT_NULL;
        _rtt_device.user_data = RT_NULL;

        rt_device_register(&_rtt_device, "rtt",
                           RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_STREAM);

        return RT_EOK;
}
/* 在 rt_components_board_init() 中执行，早于 rt_console_set_device() */
INIT_BOARD_EXPORT(_rtt_device_init);
