/*
 * Copyright (C) 2022-2025, PD-Embedded Project
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-06-25     CDT          first version
 */

#ifndef __BSP_UART_H__
#define __BSP_UART_H__

#include <bsp_config.h>
#include <hc32_ll.h>
#include <menuconfig.h>

#ifdef __cplusplus
extern "C"
{
#endif

        /* ================================================================== */
        /*  UART1 Console (Polled) */
        /* ================================================================== */

        /**
         * @brief  Initialize UART1 for polled console I/O.
         *
         *         Configures UART1 peripheral with 115200-8-N-1, internal clock
         *         source (PCLK1 = 100MHz). TX and RX enabled, no interrupts.
         *
         *         GPIO pin mapping (HC32F460 LQFP100):
         *           - PA9:  USART1_TX  (GPIO_FUNC_32)
         *           - PA10: USART1_RX  (GPIO_FUNC_33)
         */
        void bsp_uart1_init(void);

        /**
         * @brief  Polled UART1 character output.
         *
         *         Blocks until the TX data register is empty, then writes one
         * byte.
         *
         * @param  c   Character to send.
         */
        void bsp_uart1_putc(char c);

        /**
         * @brief  Polled UART1 string output.
         *
         *         Sends a buffer of len bytes via repeated bsp_uart1_putc().
         *
         * @param  str  Pointer to data buffer.
         * @param  len  Number of bytes to send.
         */
        void bsp_uart1_write(const char* str, unsigned long len);

        /**
         * @brief  Non-blocking UART1 character input.
         *
         *         Returns immediately; does not wait for data.
         *
         * @param  c   Pointer to store received character.
         * @retval  1  Character received successfully.
         * @retval  0  No data available.
         */
        int bsp_uart1_getc(char* c);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_UART_H__ */
