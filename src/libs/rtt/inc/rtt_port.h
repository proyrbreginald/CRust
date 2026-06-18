#ifndef __RTT_PORT_H__
#define __RTT_PORT_H__

#include <rtthread.h>

#ifndef RTT_UP_BUF_SIZE
#define RTT_UP_BUF_SIZE (1 * 1024)
#endif

#ifndef RTT_DOWN_BUF_SIZE
#define RTT_DOWN_BUF_SIZE 128
#endif

/**
 * RTT_CB_SECTION - 控制块放置节区
 *
 * 调试器需要在固定地址找到控制块。默认使用 ".rtt_cb" 节，
 * 可在链接脚本中指定其地址，或在外部重写此宏。
 */
#ifndef RTT_CB_SECTION
#define RTT_CB_SECTION __attribute__((section(".rtt_cb")))
#endif

/**
 * RTT_BUF_SECTION - 缓冲区放置节区
 *
 * 缓冲区内存默认放在 ".rtt_buf" 节，也可重写此宏。
 */
#ifndef RTT_BUF_SECTION
#define RTT_BUF_SECTION __attribute__((section(".rtt_buf")))
#endif

/**
 * rtt_lock - RTT 全局 spinlock
 *
 * 用于保护 RTT 控制块和缓冲区的并发访问。
 * 在 rtt_port.c 中定义并初始化，各 API 函数通过
 * rt_spin_lock_irqsave() / rt_spin_unlock_irqrestore() 使用。
 */
extern struct rt_spinlock rtt_lock;
#define RTT_LOCK rtt_lock

/**
 * rtt_port_dmb() - 数据内存屏障
 *
 * 确保内存访问顺序，防止编译器/CPU 重排。
 * 在 ARM Cortex-M 上对应 __DMB()。
 * 调试器通过 DAP 读取缓冲区时需要此屏障保证数据一致性。
 */
void rtt_port_dmb(void);

#endif /* __RTT_PORT_H__ */
