/*
 * Copyright (C) 2022-2025, PD-Embedded Project
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-06-16     CDT          first version
 */

#ifndef __BSP_H__
#define __BSP_H__

#include <bsp_config.h>
#include <hc32_ll.h>
#include <menuconfig.h>

#ifdef __cplusplus
extern "C"
{
#endif

        /* ================================================================== */
        /*  Board Initialization */
        /* ================================================================== */

        /**
         * @brief  Initialize the board.
         *         Call this once at startup, after SystemInit().
         *         Sets up clocks, caches, and board-level hardware.
         */
        void bsp_init(void);

        /**
         * @brief  Initialize system clock to high-performance mode.
         *         Configure PLL, flash wait cycles, SRAM timing, etc.
         */
        void bsp_clock_init(void);

        /* ================================================================== */
        /*  Timer / Delay */
        /* ================================================================== */

        /**
         * @brief  Initialize SysTick timer for 1ms ticks.
         */
        void bsp_timer_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_H__ */
