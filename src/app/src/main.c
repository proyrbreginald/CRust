#include <hc32_ll.h>
#include <main.h>
#include <rtt.h>

/**
 * @brief  LED Init
 * @param  None
 * @retval None
 */
static void LED_Init(void)
{
/* LED Port/Pin definition */
#define LED_G_PORT (GPIO_PORT_C)
#define LED_G_PIN (GPIO_PIN_13)
#define LED_G_TOGGLE GPIO_TogglePins(LED_G_PORT, LED_G_PIN)

        stc_gpio_init_t stcGpioInit;
        (void)GPIO_StructInit(&stcGpioInit);
        stcGpioInit.u16PinState = PIN_STAT_RST;
        stcGpioInit.u16PinDir = PIN_DIR_OUT;
        (void)GPIO_Init(LED_G_PORT, LED_G_PIN, &stcGpioInit);
}

/**
 * @brief  Main function of GPIO project
 * @param  None
 * @retval int32_t return value, if needed
 */
void main(void)
{
        rtt_write("start!\n", 7);

        /* LED initialize */
        LED_Init();

        for (;;)
        {
                LED_G_TOGGLE;
                DDL_DelayMS(500u);
                rtt_write("hello\n", 6);
        }
}