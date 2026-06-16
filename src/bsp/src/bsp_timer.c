/*
 * Copyright (C) 2022-2025, PD-Embedded Project
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-06-16     CDT          first version
 */

#include "hc32_ll_utility.h"
#include <bsp.h>

#if defined(BSP_USING_TIMER)

/* 系统滴答计数 (1ms 分辨率) */
static volatile uint64_t s_sys_tick_count = 0;

/**
 * @brief  SysTick 中断处理函数。
 *         覆盖 CMSIS 中的弱符号 SysTick_Handler。
 */
void SysTick_Handler(void) { s_sys_tick_count++; }

/**
 * @brief  初始化 SysTick 定时器，产生 1ms 中断。
 */
void bsp_timer_init(void)
{
        s_sys_tick_count = 0;

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

/**
 * @brief  tick 级阻塞延时。
 * @param  tick 延时 tick 数
 */
void bsp_delay_tick(uint32_t tick)
{
        uint64_t start = s_sys_tick_count;

        while ((s_sys_tick_count - start) < tick)
        {
                ;
        }
}

/**
 * @brief  获取当前系统滴答计数 (1ms 分辨率)。
 */
uint64_t bsp_get_tick(void) { return s_sys_tick_count; }

#endif /* BSP_USING_TIMER */
