#include <bsp.h>

/**
 * @brief 复位处理函数执行完后调用，做裸机启动前的系统性初始化。
 * @param  None
 * @retval None
 */
void startup(void)
{
        // 启用 FPU、配置向量表、更新系统时钟
        SystemInit();

        // BSP 板级初始化（时钟、UART、定时器等）
        bsp_init();

        // 进入用户主程序
        extern void main(void);
        main();
}