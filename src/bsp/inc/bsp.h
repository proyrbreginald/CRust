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
        /*  Debug UART */
        /* ================================================================== */

        /**
         * @brief  Initialize the debug UART.
         */
        void bsp_uart_init(void);

        /**
         * @brief  Send a single character via debug UART.
         */
        void bsp_uart_putchar(char c);

        /**
         * @brief  Send a string via debug UART.
         */
        void bsp_uart_write(const char* str, unsigned int len);

        /**
         * @brief  Receive a single character via debug UART (blocking).
         */
        char bsp_uart_getchar(void);

        /**
         * @brief  Check if a character is available via debug UART.
         * @return 1 if available, 0 otherwise.
         */
        int bsp_uart_available(void);

        /* ================================================================== */
        /*  Timer / Delay */
        /* ================================================================== */

        /**
         * @brief  Initialize SysTick timer for 1ms ticks.
         */
        void bsp_timer_init(void);

        /**
         * @brief  Blocking delay in milliseconds.
         */
        void bsp_delay_tick(uint32_t ms);

        /**
         * @brief  Get current system tick count (1ms resolution).
         */
        uint64_t bsp_get_tick(void);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_H__ */
