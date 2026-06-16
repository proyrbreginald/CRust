/*
 * Copyright (C) 2022-2025, PD-Embedded Project
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-06-16     CDT          first version
 */

#ifndef __BSP_CONFIG_H__
#define __BSP_CONFIG_H__

#include <menuconfig.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* ---------- XTAL ---------- */
#define BSP_XTAL_PORT (GPIO_PORT_H)
#define BSP_XTAL_IN_PIN (GPIO_PIN_01)
#define BSP_XTAL_OUT_PIN (GPIO_PIN_00)

/* ---------- LED: PC13 (低电平亮) ---------- */
#define BSP_LED_PORT (GPIO_PORT_C)
#define BSP_LED_PIN (GPIO_PIN_13)
#define BSP_LED_ON GPIO_ResetPins(BSP_LED_PORT, BSP_LED_PIN)
#define BSP_LED_OFF GPIO_SetPins(BSP_LED_PORT, BSP_LED_PIN)
#define BSP_LED_TOGGLE GPIO_TogglePins(BSP_LED_PORT, BSP_LED_PIN)

#ifdef __cplusplus
}
#endif

#endif /* __BSP_CONFIG_H__ */