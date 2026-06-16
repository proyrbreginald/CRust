/*
 * Copyright (C) 2022-2025, PD-Embedded Project
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-06-16     CDT          first version
 */

#include <bsp.h>

/**
 * @brief  Unlock peripheral registers for write access.
 */
static void bsp_periph_unlock(void) { LL_PERIPH_WE(LL_PERIPH_ALL); }

/**
 * @brief  Initialize system clock to high-performance mode.
 *
 *         Clock tree:
 *           XTAL (8MHz) → MPLL → PLLP (200MHz)
 *
 *         Bus dividers:
 *           HCLK  = 200MHz  (div1)
 *           PCLK0 = 200MHz  (div1)
 *           PCLK1 = 100MHz  (div2)
 *           PCLK2 = 50MHz   (div4)
 *           PCLK3 = 50MHz   (div4)
 *           PCLK4 = 100MHz  (div2)
 *           EXCLK = 100MHz  (div2)
 */
void bsp_clock_init(void)
{
        stc_clock_xtal_init_t stcXtalInit;
        stc_clock_pll_init_t stcMpllInit;

        GPIO_SetReadWaitCycle(GPIO_RD_WAIT3);

        /* Enable XTAL pins */
        GPIO_AnalogCmd(BSP_XTAL_PORT, BSP_XTAL_IN_PIN | BSP_XTAL_OUT_PIN,
                       ENABLE);

        /* Set bus clock dividers */
        CLK_SetClockDiv(CLK_BUS_CLK_ALL,
                        (CLK_HCLK_DIV1 | CLK_EXCLK_DIV2 | CLK_PCLK0_DIV1 |
                         CLK_PCLK1_DIV2 | CLK_PCLK2_DIV4 | CLK_PCLK3_DIV4 |
                         CLK_PCLK4_DIV2));

        /* Configure and enable external XTAL (8MHz) */
        (void)CLK_XtalStructInit(&stcXtalInit);
        stcXtalInit.u8Mode = CLK_XTAL_MD_OSC;
        stcXtalInit.u8Drv = CLK_XTAL_DRV_ULOW;
        stcXtalInit.u8State = CLK_XTAL_ON;
        stcXtalInit.u8StableTime = CLK_XTAL_STB_2MS;
        (void)CLK_XtalInit(&stcXtalInit);

        /* Actually use: 8MHz / 1 * 50 / 2 = 200MHz */
        (void)CLK_PLLStructInit(&stcMpllInit);
        stcMpllInit.u8PLLState = CLK_PLL_ON;
        stcMpllInit.PLLCFGR = 0UL;
        stcMpllInit.PLLCFGR_f.PLLM = 1UL - 1UL;  /* /1 */
        stcMpllInit.PLLCFGR_f.PLLN = 50UL - 1UL; /* *50 → 400MHz VCO */
        stcMpllInit.PLLCFGR_f.PLLP = 2UL - 1UL;  /* /2 → 200MHz */
        stcMpllInit.PLLCFGR_f.PLLQ = 2UL - 1UL;
        stcMpllInit.PLLCFGR_f.PLLR = 2UL - 1UL;
        stcMpllInit.PLLCFGR_f.PLLSRC = CLK_PLL_SRC_XTAL;
        (void)CLK_PLLInit(&stcMpllInit);

        /* Wait for PLL ready */
        while (SET != CLK_GetStableStatus(CLK_STB_FLAG_PLL))
        {
                ;
        }

        /* SRAM wait cycle */
        SRAM_SetWaitCycle(SRAM_SRAMH, SRAM_WAIT_CYCLE0, SRAM_WAIT_CYCLE0);
        SRAM_SetWaitCycle((SRAM_SRAM12 | SRAM_SRAM3 | SRAM_SRAMR),
                          SRAM_WAIT_CYCLE1, SRAM_WAIT_CYCLE1);

        /* Flash wait cycle */
        (void)EFM_SetWaitCycle(EFM_WAIT_CYCLE5);
        GPIO_SetReadWaitCycle(GPIO_RD_WAIT3);

        /* Switch to high-performance mode */
        (void)PWC_HighSpeedToHighPerformance();

        /* Switch system clock to MPLL */
        CLK_SetSysClockSrc(CLK_SYSCLK_SRC_PLL);

        /* Enable Flash cache */
        EFM_CacheRamReset(ENABLE);
        EFM_CacheRamReset(DISABLE);
        EFM_CacheCmd(ENABLE);
}

/**
 * @brief  Initialize the board.
 *
 *         Call this once at startup, after SystemInit().
 *         Performs clock init, peripheral unlock, and
 *         board-level hardware setup.
 */
void bsp_init(void)
{
        /* Unlock peripheral registers */
        bsp_periph_unlock();

        /* Initialize system clock */
        bsp_clock_init();

        /* Initialize SysTick timer */
#if defined(BSP_USING_TIMER)
        bsp_timer_init();
#endif
}
