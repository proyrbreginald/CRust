#include <bsp.h>
#include <hc32_ll.h>
#include <rtthread.h>

#if defined(BSP_USING_TIMER)

/**
 * @brief  SysTick 中断处理函数。
 *         覆盖 CMSIS 中的弱符号 SysTick_Handler。
 *         同时喂给 RT-Thread 内核时钟。
 */
void SysTick_Handler(void)
{
        /* enter interrupt (RT-Thread nesting tracking) */
        rt_interrupt_enter();

        rt_tick_increase();

        /* leave interrupt */
        rt_interrupt_leave();
}

/**
 * @brief  初始化 SysTick 定时器，产生 1ms 中断。
 */
void bsp_timer_init(void)
{
        /* 配置 SysTick: 重载值 = 系统时钟 / 1000 */
        /* SystemCoreClock 在 SystemInit() 中已更新 */
        if (SysTick_Init(1000))
        {
                /* 配置失败 — 死循环 */
                while (1)
                {
                        ;
                }
        }

        /* SysTick 中断优先级设为最低 */
        NVIC_SetPriority(SysTick_IRQn, 0xFFU);
}

#endif /* BSP_USING_TIMER */