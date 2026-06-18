#include <rtt_port.h>

/**
 * rtt_lock - RTT 全局 spinlock
 *
 * 保护 RTT 控制块和环形缓冲区的并发访问。
 * 在 RT-Thread 调度器启动前即可使用（spinlock 不依赖调度器）。
 */
struct rt_spinlock rtt_lock = RT_SPINLOCK_INIT;

/**
 * rtt_port_dmb() - 数据内存屏障
 *
 * 防止编译器和 CPU 重排内存访问。
 * 在 ARMv7-M 上对应 DMB 指令，确保调试器（通过 DAP）读到一致的数据。
 */
void rtt_port_dmb(void) { __asm volatile("DMB" : : : "memory"); }
