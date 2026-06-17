#include <bsp.h>
#include <rtthread.h>

/* rtthread_startup() 在 RT-Thread 内核的 components.c 中定义，
 * 但未在公开头文件中声明，此处显式 extern */
extern int rtthread_startup(void);

/**
 * @brief 复位处理函数执行完后调用。
 *        现在由 RT-Thread 接管启动流程：
 *        SystemInit() → rtthread_startup() → rt_hw_board_init()
 *        → ... → main()
 * @param  None
 * @retval None
 */
void startup(void)
{
        // 启用 FPU、配置向量表、更新系统时钟
        SystemInit();

        // RT-Thread 启动（含板级初始化、调度器、主线程等）
        rtthread_startup();
}