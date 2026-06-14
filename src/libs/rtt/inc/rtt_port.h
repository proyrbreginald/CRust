/*
 * Copyright (C) 2025, Reginald
 *
 * rtt_port.h - RTT 移植层接口
 *
 * 提供平台无关的抽象层，适配不同 MCU 和编译器。
 * 用户需根据目标平台实现以下原语：
 *   1. 临界区保护（中断开关）
 *   2. 内存屏障（DMB/DSB）
 *   3. 缓冲区内存声明
 */

#ifndef __RTT_PORT_H__
#define __RTT_PORT_H__

#include <stdint.h>

/* ------------------------------------------------------------------ */
/*  缓冲区大小配置（可根据需求修改）                                     */
/* ------------------------------------------------------------------ */

#ifndef RTT_UP_BUF_SIZE
#define RTT_UP_BUF_SIZE 1024 /* 上行缓冲区：适合日志输出 */
#endif

#ifndef RTT_DOWN_BUF_SIZE
#define RTT_DOWN_BUF_SIZE 128 /* 下行缓冲区：适合命令输入 */
#endif

/* ------------------------------------------------------------------ */
/*  内存布局                                                           */
/* ------------------------------------------------------------------ */

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

/* ------------------------------------------------------------------ */
/*  平台原语（必须由移植层实现）                                         */
/* ------------------------------------------------------------------ */

/**
 * rtt_port_irq_save() - 保存中断状态并全局关中断
 *
 * 返回值：保存的中断状态字，后续传给 rtt_port_irq_restore()。
 */
unsigned long rtt_port_irq_save(void);

/**
 * rtt_port_irq_restore() - 恢复中断状态
 * @flags:	rtt_port_irq_save() 返回的状态字
 */
void rtt_port_irq_restore(unsigned long flags);

/**
 * rtt_port_dmb() - 数据内存屏障
 *
 * 确保内存访问顺序，防止编译器/CPU 重排。
 * 在 ARM Cortex-M 上对应 __DMB()。
 */
void rtt_port_dmb(void);

#endif /* __RTT_PORT_H__ */
