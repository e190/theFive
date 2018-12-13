/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-5      SummerGift   change to new framework
 */

#ifndef __BOARD_H__
#define __BOARD_H__

#include <rtthread.h>
#include <stm32f4xx.h>
#include "drv_common.h"

#define SOFTWARE_VERSION     "1.07.15"
#define HARDWARE_VERSION     "1.2"

#define BSP_USING_GPIO
#define BSP_USING_UART1
#define BSP_USING_UART2
#define BSP_USING_UART3
#define BSP_USING_UART4
#define BSP_UART_USING_DMA_RX

#define BSP_USING_SPI2

#define BSP_USING_I2C1
#define BSP_I2C1_SCL_PIN 73
#define BSP_I2C1_SDA_PIN 96
#define BSP_USING_I2C2
#define BSP_I2C2_SCL_PIN 136
#define BSP_I2C2_SDA_PIN 137
#define BSP_USING_I2C3
#define BSP_I2C3_SCL_PIN 11
#define BSP_I2C3_SDA_PIN 10
#define BSP_USING_I2C4
#define BSP_I2C4_SCL_PIN 93
#define BSP_I2C4_SDA_PIN 97
#define BSP_USING_I2C5
#define BSP_I2C5_SCL_PIN 114
#define BSP_I2C5_SDA_PIN 115

#define BSP_USING_PWM1
#define BSP_USING_PWM1_CH1
#define BSP_USING_PWM1_CH2
#define BSP_USING_PWM1_CH3
#define BSP_USING_PWM1_CH4
#define BSP_USING_PWM4
#define BSP_USING_PWM4_CH1
#define BSP_USING_PWM4_CH2
#define BSP_USING_PWM4_CH3
#define BSP_USING_PWM4_CH4

#ifdef BSP_USING_GPIO
#include "drv_gpio.h"
/* Board Pin definitions */
#define LED0_PIN                       GET_PIN(F,  5)
#define LED1_PIN                       GET_PIN(F, 10)
#endif

#define STM32_SRAM_SIZE      (128)
#define STM32_SRAM_END       (0x20000000 + STM32_SRAM_SIZE * 1024)

#ifdef __CC_ARM
extern int Image$$RW_IRAM1$$ZI$$Limit;
#define HEAP_BEGIN      ((void *)&Image$$RW_IRAM1$$ZI$$Limit)
#elif __ICCARM__
#pragma section="CSTACK"
#define HEAP_BEGIN      (__segment_end("CSTACK"))
#else
extern int __bss_end;
#define HEAP_BEGIN      ((void *)&__bss_end)
#endif

#define HEAP_END        STM32_SRAM_END

void SystemClock_Config(void);
void MX_GPIO_Init(void);

#endif

