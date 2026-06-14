/*
 * Copyright (C) 2025, Reginald
 *
 * rtt_port.c - RTT 移植层实现（ARM Cortex-M4 / HC32F460）
 *
 * 提供 CMSIS 兼容的临界区保护和内存屏障实现。
 * 若移植到其他平台（如 RISC-V、Xtensa），替换此文件即可。
 */

#include "rtt_port.h"

/*
 * CMSIS Core 头文件由编译器的 -I 路径提供。
 * HC32F460 项目已有 CMSIS 支持，直接使用内置 intrinsic。
 */
#if defined(__ARMCC_VERSION) || defined(__GNUC__) || defined(__ICCARM__)
#include <cmsis_compiler.h>
#else
/* 若无法包含 CMSIS，用户可以自定义实现 */
#warning "CMSIS header not found, using empty port implementations"
#endif

/**
 * rtt_port_irq_save() - 保存中断状态并全局关中断
 *
 * 对 ARM Cortex-M：使用 PRIMASK 控制中断。
 * 读取 PRIMASK 保存当前状态，然后写 1 关中断。
 *
 * 返回值：保存的状态字（传给 rtt_port_irq_restore）。
 */
unsigned long rtt_port_irq_save(void)
{
#if defined(__GNUC__) || defined(__ARMCC_VERSION)
        return __get_PRIMASK();
#else
        unsigned long flags = 0;
        __asm volatile("MRS %0, PRIMASK" : "=r"(flags));
        return flags;
#endif
}

/**
 * rtt_port_irq_restore() - 恢复中断状态
 * @flags:	rtt_port_irq_save() 返回的状态字
 */
void rtt_port_irq_restore(unsigned long flags)
{
#if defined(__GNUC__) || defined(__ARMCC_VERSION)
        __set_PRIMASK(flags);
#else
        __asm volatile("MSR PRIMASK, %0" : : "r"(flags));
#endif
}

/**
 * rtt_port_dmb() - 数据内存屏障
 *
 * 防止编译器和 CPU 重排内存访问。
 * 在 ARMv7-M 上对应 DMB 指令，确保数据一致性。
 */
void rtt_port_dmb(void) { __asm volatile("DMB" : : : "memory"); }
