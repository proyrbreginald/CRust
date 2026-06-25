/*
 * Copyright (C) 2022-2025, PD-Embedded Project
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-06-25     CDT          first version
 */

#include <bsp_uart.h>

/* ================================================================== */
/*  Local Constants                                                    */
/* ================================================================== */

/* UART1 baudrate for console */
#define BSP_UART1_BAUDRATE (115200UL)

/*
 * USART1 pin mapping (HC32F460 LQFP100):
 *   - PA09: USART1_TX GPIO_FUNC_32
 *   - PA10: USART1_RX GPIO_FUNC_33
 */
#define USART1_TX_PORT (GPIO_PORT_A)
#define USART1_TX_PIN (GPIO_PIN_09)
#define USART1_TX_GPIO_FUNC (GPIO_FUNC_32)

#define USART1_RX_PORT (GPIO_PORT_A)
#define USART1_RX_PIN (GPIO_PIN_10)
#define USART1_RX_GPIO_FUNC (GPIO_FUNC_33)

/* ================================================================== */
/*  UART1 Initialization                                               */
/* ================================================================== */

/**
 * @brief  Initialize UART1 for polled console I/O (115200-8-N-1).
 *
 *         Clock source: PCLK1 = 100MHz (internal clock).
 *         Both TX and RX enabled in polled mode (no interrupts).
 *
 *         Follows the DDL example pattern:
 *         GPIO_SetFunc → FCG clock enable → USART_UART_Init → FuncCmd
 */
void bsp_uart1_init(void)
{
        stc_usart_uart_init_t stcUartInit;

        /* Configure USART1 RX/TX pins (DDL example pattern) */
        GPIO_SetFunc(USART1_RX_PORT, USART1_RX_PIN, USART1_RX_GPIO_FUNC);
        GPIO_SetFunc(USART1_TX_PORT, USART1_TX_PIN, USART1_TX_GPIO_FUNC);

        /* Enable UART1 peripheral clock */
        FCG_Fcg1PeriphClockCmd(FCG1_PERIPH_USART1, ENABLE);

        /* De-initialize to a known state */
        (void)USART_DeInit(CM_USART1);

        /* Fill default UART init structure, then override key fields */
        (void)USART_UART_StructInit(&stcUartInit);
        stcUartInit.u32ClockDiv = USART_CLK_DIV64;
        stcUartInit.u32Baudrate = BSP_UART1_BAUDRATE;
        stcUartInit.u32OverSampleBit = USART_OVER_SAMPLE_8BIT;
        /* Other fields keep StructInit defaults (8-N-1, LSB first, no flow
         * control) */

        /* USART_UART_Init internally configures clock, baudrate BRR, and format
         */
        (void)USART_UART_Init(CM_USART1, &stcUartInit, NULL);

        /* Enable TX and RX (polled mode, no interrupts) */
        USART_FuncCmd(CM_USART1, (USART_RX | USART_TX), ENABLE);
}

/* ================================================================== */
/*  Polled TX Functions                                                */
/* ================================================================== */

/**
 * @brief  Send one character via UART1 (polled, blocking).
 *
 *         Waits until the TX data register is empty, then writes.
 */
void bsp_uart1_putc(char c)
{
        /* Wait for TX data register empty (TXE flag) */
        while (SET != USART_GetStatus(CM_USART1, USART_FLAG_TX_EMPTY))
        {
                /* spin */
        }
        USART_WriteData(CM_USART1, (uint16_t)(unsigned char)c);
}

/**
 * @brief  Send a buffer via UART1 (polled, blocking).
 */
void bsp_uart1_write(const char* str, unsigned long len)
{
        unsigned long i;
        for (i = 0UL; i < len; i++)
        {
                bsp_uart1_putc(str[i]);
        }
}

/* ================================================================== */
/*  Polled RX — Software Ring Buffer                                   */
/* ================================================================== */

/*
 * A small ring buffer that absorbs multi-byte bursts (terminal escape
 * sequences, paste operations) in a single drain pass.  Without this
 * buffer the HC32F460's single-byte RDR overruns when a second byte
 * arrives before Finsh issues the next rt_device_read().
 */
#define UART1_RX_BUF_SIZE (64U)
static char          uart1_rx_buf[UART1_RX_BUF_SIZE];
static unsigned int  uart1_rx_head; /* next write position */
static unsigned int  uart1_rx_tail; /* next read position  */

/**
 * @brief  Drain all available bytes from the UART RDR into the ring buffer.
 *
 *         Reads the status register once, then loops to pull every
 *         ready character out of the hardware.  Error flags (ORE/FE/PE)
 *         are cleared on the fly so the RX path never stalls.
 *
 *         Call this at the beginning of every read cycle — it ensures
 *         that multi-byte sequences (e.g. \x1b[A) are captured in one
 *         burst before Finsh requests them byte-by-byte.
 */
static void uart1_drain(void)
{
        uint32_t sr;
        char     ch;

        for (;;)
        {
                sr = READ_REG32(CM_USART1->SR);

                /* Error recovery: overrun / frame / parity ---------------- */
                if (0UL != (sr & (USART_SR_ORE | USART_SR_FE | USART_SR_PE)))
                {
                        (void)USART_ReadData(CM_USART1);
                        USART_ClearStatus(CM_USART1,
                                          USART_FLAG_OVERRUN |
                                                  USART_FLAG_FRAME_ERR |
                                                  USART_FLAG_PARITY_ERR);
                        continue;
                }

                /* Normal data --------------------------------------------- */
                if (0UL != (sr & USART_SR_RXNE))
                {
                        ch = (char)(uint8_t)USART_ReadData(CM_USART1);
                        uart1_rx_buf[uart1_rx_head] = ch;
                        uart1_rx_head =
                                (uart1_rx_head + 1U) % UART1_RX_BUF_SIZE;
                        /*
                         * If the buffer is full, advance the tail (drop the
                         * oldest byte) to make room.  This is safe because
                         * RT-Thread Finsh never fills more than a handful
                         * of bytes between drain calls.
                         */
                        if (uart1_rx_head == uart1_rx_tail)
                        {
                                uart1_rx_tail = (uart1_rx_tail + 1U) %
                                                UART1_RX_BUF_SIZE;
                        }
                        continue;
                }

                /* No more data — exit the drain loop */
                break;
        }
}

/**
 * @brief  Try to receive one character from UART1 (non-blocking).
 *
 *         Reads from the software ring buffer first; if empty,
 *         drains the UART RDR and tries again.
 *
 * @param  c   Pointer to store received character.
 * @retval  1  Character received successfully.
 * @retval  0  No data available.
 */
int bsp_uart1_getc(char* c)
{
        /* Ring buffer hit — zero-latency return */
        if (uart1_rx_tail != uart1_rx_head)
        {
                *c = uart1_rx_buf[uart1_rx_tail];
                uart1_rx_tail = (uart1_rx_tail + 1U) % UART1_RX_BUF_SIZE;
                return 1;
        }

        /* Buffer empty — pull hardware, then try again */
        uart1_drain();
        if (uart1_rx_tail != uart1_rx_head)
        {
                *c = uart1_rx_buf[uart1_rx_tail];
                uart1_rx_tail = (uart1_rx_tail + 1U) % UART1_RX_BUF_SIZE;
                return 1;
        }

        return 0;
}
