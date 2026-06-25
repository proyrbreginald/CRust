#include <bsp.h>
#include <hc32_ll.h>
#include <rthw.h>
#include <rtthread.h>
#include <rtt.h>
#include <symbol.h>

#ifdef BSP_CONSOLE_VIA_UART1
#include <bsp_uart.h>
#endif

/* ================================================================== */
/*  UART1 Console Device (polled, write-only)                           */
/* ================================================================== */

#ifdef BSP_CONSOLE_VIA_UART1

static struct rt_device _uart1_device;

static rt_err_t _uart1_dev_init(rt_device_t dev)
{
        (void)dev;
        return RT_EOK;
}

static rt_err_t _uart1_dev_open(rt_device_t dev, rt_uint16_t oflag)
{
        (void)dev;
        (void)oflag;
        return RT_EOK;
}

static rt_err_t _uart1_dev_close(rt_device_t dev)
{
        (void)dev;
        return RT_EOK;
}

static rt_ssize_t _uart1_dev_read(rt_device_t dev, rt_off_t pos, void *buffer,
                                  rt_size_t size)
{
        char *p;
        int    ret;

        (void)dev;
        (void)pos;

        p = (char *)buffer;

        /*
         * 1) 优先从软件环形缓冲区获取（bsp_uart1_getc 内部会先 drain
         *    UART RDR 到缓冲区，确保多字节序列如 \x1b[A 被完整捕获）。
         */
        ret = bsp_uart1_getc(p);
        if (ret > 0)
                return (rt_ssize_t)ret;

        /*
         * 2) 短暂自旋：数据可能正在线路上到达（~87μs/byte@115200）。
         */
        {
                int spin;
                for (spin = 0; spin < 500; spin++)
                {
                        ret = bsp_uart1_getc(p);
                        if (ret > 0)
                                return (rt_ssize_t)ret;
                }
        }

        /*
         * 3) 长轮询等待（每 1 tick = 1ms 让出 CPU）。
         *
         * 为什么不能返回 0？
         *   Finsh 的 finsh_getchar() 在 rt_device_read() 返回值 != 1
         *   时会永久阻塞在 rx_sem 上（RT_WAITING_FOREVER），而 polled
         *   UART 没有中断触发 rx_indicate 释放该信号量。
         */
        for (;;) {
                rt_thread_mdelay(1);
                ret = bsp_uart1_getc(p);
                if (ret > 0)
                        return (rt_ssize_t)ret;
        }
}

static rt_ssize_t _uart1_dev_write(rt_device_t dev, rt_off_t pos,
                                   const void *buffer, rt_size_t size)
{
        (void)dev;
        (void)pos;
        bsp_uart1_write((const char *)buffer, (unsigned long)size);
        return (rt_ssize_t)size;
}

static rt_err_t _uart1_dev_control(rt_device_t dev, int cmd, void *args)
{
        (void)dev;
        (void)cmd;
        (void)args;
        return RT_EOK;
}

/**
 * @brief  Register UART1 as an RT-Thread character device ("uart1").
 *
 *         This allows rt_console_set_device("uart1") to find and use
 *         UART1 for rt_kprintf / Finsh I/O.
 *
 *         TX: polled, blocking write.
 *         RX: polled, blocking read (driver-internal polling loop,
 *             compatible with Finsh's blocking read model).
 */
static int _uart1_device_init(void)
{
        /* Initialize the UART1 peripheral (115200-8-N-1, polled TX/RX) */
        bsp_uart1_init();

        _uart1_device.type        = RT_Device_Class_Char;
        _uart1_device.init        = _uart1_dev_init;
        _uart1_device.open        = _uart1_dev_open;
        _uart1_device.close       = _uart1_dev_close;
        _uart1_device.read        = _uart1_dev_read;
        _uart1_device.write       = _uart1_dev_write;
        _uart1_device.control     = _uart1_dev_control;
        _uart1_device.rx_indicate = RT_NULL;
        _uart1_device.tx_complete = RT_NULL;
        _uart1_device.user_data   = RT_NULL;

        rt_device_register(&_uart1_device, "uart1",
                           RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_STREAM);

        return RT_EOK;
}
/* Register before rt_console_set_device() */
INIT_BOARD_EXPORT(_uart1_device_init);

#endif /* BSP_CONSOLE_VIA_UART1 */

/* ================================================================== */
/*  RT-Thread Board Init                                                */
/* ================================================================== */

/**
 * @brief  RT-Thread board initialization.
 *         Called by rtthread_startup() before the scheduler starts.
 *
 *         Delegates to the project's bsp_init() for clock, SysTick,
 *         and basic hardware setup, then initializes the RT-Thread heap.
 *
 *         Note: Console character device is registered via
 *         INIT_BOARD_EXPORT (rtt_device.c for RTT, or _uart1_device_init
 *         above for UART1), which runs inside rt_components_board_init()
 *         below, before rt_console_set_device().
 */
void rt_hw_board_init(void)
{
        /* Project BSP initialization: clock, SysTick, etc. */
        bsp_init();

        /* RT-Thread heap initialization */
        rt_system_heap_init((void*)_heap_start, (void*)_heap_end);

#ifdef RT_USING_COMPONENTS_INIT
        /* Invoke board-level INIT_BOARD_EXPORT() routines
         * (includes rtt_device / uart1_device registration) */
        rt_components_board_init();
#endif

#ifdef RT_USING_CONSOLE
        /* Set the console device (e.g. "rtt" or "uart1") */
        rt_console_set_device(RT_CONSOLE_DEVICE_NAME);
#endif
}

/**
 * @brief  Microsecond delay using SysTick.
 *         Commonly used by RT-Thread drivers.
 */
void rt_hw_us_delay(rt_uint32_t us)
{
        rt_uint32_t start, now, delta, reload, us_tick;

        start = SysTick->VAL;
        reload = SysTick->LOAD;
        us_tick = SystemCoreClock / 1000000UL;

        do
        {
                now = SysTick->VAL;
                delta = (start > now) ? (start - now) : (reload + start - now);
        } while (delta < us_tick * us);
}

/**
 * @brief  Console output fallback.
 *         Called by _kputs() when no console device is registered.
 *         Without this, rt_kprintf silently produces no output.
 */
void rt_hw_console_output(const char *str, long len)
{
#ifdef BSP_CONSOLE_VIA_UART1
        bsp_uart1_write(str, (unsigned long)len);
#else
        rtt_write(str, (size_t)len);
#endif
}