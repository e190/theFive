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

#define SOFTWARE_VERSION     "1.12.19"
#define HARDWARE_VERSION     "2.0.1"

#define BSP_USING_ON_CHIP_FLASH

#define BSP_USING_GPIO
#define BSP_USING_UART1
#define BSP_USING_UART2
#define BSP_USING_UART3
#define BSP_USING_UART4
#define BSP_UART_USING_DMA_RX

#define BSP_USING_SPI2

#define BSP_USING_I2C1
#define BSP_I2C1_SCL_PIN 28
#define BSP_I2C1_SDA_PIN 38
#define BSP_USING_I2C2
#define BSP_I2C2_SCL_PIN 22
#define BSP_I2C2_SDA_PIN 23
#define BSP_USING_I2C3
#define BSP_I2C3_SCL_PIN 81
#define BSP_I2C3_SDA_PIN 80
#define BSP_USING_I2C4
#define BSP_I2C4_SCL_PIN 104
#define BSP_I2C4_SDA_PIN 39
#define BSP_USING_I2C5
#define BSP_I2C5_SCL_PIN 48
#define BSP_I2C5_SDA_PIN 49

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

#define BSP_USING_SDIO
#define BSP_USING_SDCARD

#ifdef BSP_USING_GPIO
#include "drv_gpio.h"
/* Board Pin definitions */
#define TEST_PIN                       GET_PIN(F,  5)
#define RUN_LED_PIN                    GET_PIN(D, 3)
#define BUZZER_PIN                     GET_PIN(B, 15)
#define SD_IN_PIN                      GET_PIN(D, 11)  //SD¿¨¼ì²â½Å

#define LED1_gpio     	GET_PIN(D, 6)
#define LED2_gpio     	GET_PIN(D, 7)
#define LED3_gpio    	GET_PIN(G, 10)
#define LED4_gpio     	GET_PIN(G, 11)
#define LED5_gpio     	GET_PIN(G, 12)
#define LED6_gpio     	GET_PIN(G, 13)
#define LED7_gpio    	GET_PIN(G, 14)
#define LED8_gpio     	GET_PIN(G, 15)

#define CupSense1_gpio     GET_PIN(F, 2)
#define CupSense2_gpio     GET_PIN(C, 13)
#define CupSense3_gpio     GET_PIN(E, 6)
#define CupSense4_gpio     GET_PIN(E, 5)
#endif

#define STM32_SRAM_SIZE      (128)
#define STM32_SRAM_END       (0x20000000 + STM32_SRAM_SIZE * 1024)

#define STM32_FLASH_START_ADRESS     ((uint32_t)0x08000000)
#define STM32_FLASH_SIZE             (512 * 1024)
#define STM32_FLASH_END_ADDRESS      ((uint32_t)(STM32_FLASH_START_ADRESS + STM32_FLASH_SIZE))

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

