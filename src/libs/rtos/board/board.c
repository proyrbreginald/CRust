#include <bsp.h>
#include <hc32_ll.h>
#include <rthw.h>
#include <rtthread.h>
#include <rtt.h>
#include <symbol.h>

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
 *         Note: RTT character device is registered via INIT_BOARD_EXPORT
 *         in rtt_device.c, which runs inside rt_components_board_init()
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
         * (includes rtt_device registration) */
        rt_components_board_init();
#endif

#ifdef RT_USING_CONSOLE
        /* Set the console device (e.g. "rtt") */
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
        rtt_write(str, (size_t)len);
}