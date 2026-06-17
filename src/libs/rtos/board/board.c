#include <bsp.h>
#include <hc32_ll.h>
#include <rthw.h>
#include <rtthread.h>
#include <symbol.h>

/**
 * @brief  RT-Thread board initialization.
 *         Called by rtthread_startup() before the scheduler starts.
 *
 *         Delegates to the project's bsp_init() for clock, SysTick,
 *         and basic hardware setup, then initializes the RT-Thread heap.
 */
void rt_hw_board_init(void)
{
        /* Project BSP initialization: clock, SysTick, etc. */
        bsp_init();

        /* RT-Thread heap initialization */
        rt_system_heap_init((void*)_heap_start, (void*)_heap_end);

#ifdef RT_USING_COMPONENTS_INIT
        /* Invoke board-level INIT_BOARD_EXPORT() routines */
        rt_components_board_init();
#endif

#ifdef RT_USING_CONSOLE
        /* Set the console device (e.g. "uart1") */
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
