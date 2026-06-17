/*
 * Copyright (c) 2025, PD-Embedded Project
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Auto-generated RT-Thread configuration file.
 * This file is included by -include rtconfig.h in the build system.
 */

#ifndef __RT_CONFIG_H__
#define __RT_CONFIG_H__

/* ========== RT-Thread Kernel ========== */
#define RT_NAME_MAX                    8
#define RT_ALIGN_SIZE                  8
#define RT_CPUS_NR                     1
#define RT_THREAD_PRIORITY_32
#define RT_THREAD_PRIORITY_MAX         32
#define RT_TICK_PER_SECOND             1000
#define RT_USING_OVERFLOW_CHECK
#define RT_USING_HOOK
#define RT_HOOK_USING_FUNC_PTR
#define RT_USING_IDLE_HOOK
#define RT_IDLE_HOOK_LIST_SIZE         4
#define IDLE_THREAD_STACK_SIZE         256
#define RT_BACKTRACE_LEVEL_MAX_NR      32

/* ========== Debug ========== */
#define RT_USING_DEBUG
#define RT_DEBUGING_ASSERT
#define RT_DEBUGING_COLOR
#define RT_DEBUGING_CONTEXT

/* ========== Inter-Thread Communication ========== */
#define RT_USING_SEMAPHORE
#define RT_USING_MUTEX
#define RT_USING_EVENT
#define RT_USING_MAILBOX
#define RT_USING_MESSAGEQUEUE

/* ========== Memory Management ========== */
#define RT_USING_MEMPOOL
#define RT_USING_SMALL_MEM
#define RT_USING_SMALL_MEM_AS_HEAP
#define RT_USING_HEAP

/* ========== Device ========== */
// #define RT_USING_DEVICE
// #define RT_USING_CONSOLE
// #define RT_CONSOLEBUF_SIZE             128
// #define RT_CONSOLE_DEVICE_NAME         "rtt"

/* ========== Components ========== */
// #define RT_USING_COMPONENTS_INIT
#define RT_USING_USER_MAIN
#define RT_MAIN_THREAD_STACK_SIZE      2048
#define RT_MAIN_THREAD_PRIORITY        10

/* ========== Finsh / MSH ========== */
// #define RT_USING_MSH
// #define RT_USING_FINSH
// #define FINSH_USING_MSH
// #define FINSH_THREAD_NAME              "tshell"
// #define FINSH_THREAD_PRIORITY          20
// #define FINSH_THREAD_STACK_SIZE        4096
// #define FINSH_USING_HISTORY
// #define FINSH_HISTORY_LINES            5
// #define FINSH_USING_SYMTAB
// #define FINSH_CMD_SIZE                 80
// #define MSH_USING_BUILT_IN_COMMANDS
// #define FINSH_USING_DESCRIPTION
// #define FINSH_ARG_MAX                  10

/* ========== Device Drivers ========== */
// #define RT_USING_DEVICE_IPC
// #define RT_UNAMED_PIPE_NUMBER          64
// #define RT_USING_SYSTEM_WORKQUEUE
// #define RT_SYSTEM_WORKQUEUE_STACKSIZE  2048
// #define RT_SYSTEM_WORKQUEUE_PRIORITY   23

/* ========== Architecture ========== */
#define RT_USING_HW_ATOMIC
#define RT_USING_CPU_FFS
#define ARCH_ARM
#define ARCH_ARM_CORTEX_M
#define ARCH_ARM_CORTEX_M4

/* ========== Version ========== */
#define RT_VER_NUM                     0x50202

#endif /* __RT_CONFIG_H__ */
