/*
 * File      : board.h
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-09-22     Bernard      add board.h to this bsp
 */
// <<< Use Configuration Wizard in Context Menu >>>
#ifndef __BOARD_H__
#define __BOARD_H__
#include <stm32f4xx.h>
#include <stm32f4xx_hal.h>
#include <rtthread.h>

#define SOFTWARE_VERSION     "1.07.15"
#define HARDWARE_VERSION     "1.2"

#if defined(SOC_STM32F407ZE)

    #define STM32F4xx_PIN_NUMBERS 144
    #define STM32_SRAM_SIZE (192-64)

#endif

#define STM32_SRAM_END          (0x20000000 + STM32_SRAM_SIZE * 1024)

#ifdef __CC_ARM
    extern int Image$$RW_IRAM1$$ZI$$Limit;
    #define HEAP_BEGIN  ((void *)&Image$$RW_IRAM1$$ZI$$Limit)
#elif __ICCARM__
    #pragma section="HEAP"
    #define HEAP_BEGIN  (__segment_end("HEAP"))
#else
    extern int __bss_end__;
    #define HEAP_BEGIN  ((void *)&__bss_end__)
#endif

#define HEAP_END    STM32_SRAM_END
extern void rt_hw_board_init(void);
#endif

