#include <bsp.h>
#include <main.h>
#include <rtthread.h>

/**
 * @brief  LED Init — 使用 BSP 定义的引脚宏
 * @param  None
 * @retval None
 */
static void LED_Init(void)
{
        stc_gpio_init_t stcGpioInit;
        (void)GPIO_StructInit(&stcGpioInit);
        stcGpioInit.u16PinState = PIN_STAT_RST;
        stcGpioInit.u16PinDir = PIN_DIR_OUT;
        (void)GPIO_Init(BSP_LED_PORT, BSP_LED_PIN, &stcGpioInit);
}

/**
 * @brief  Main function — 裸机主循环
 * @param  None
 * @retval None
 */
void main(void)
{
        /* LED initialize */
        LED_Init();

        for (;;)
        {
                BSP_LED_TOGGLE;
                rt_thread_mdelay(1000u);
                // rt_kprintf("hello\n");
        }
}