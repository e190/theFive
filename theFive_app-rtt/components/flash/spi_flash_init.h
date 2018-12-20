/*
 * File      : drv_spiflash.h
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2015, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author            Notes
 * 2017-11-08     ZYH            the first version
 */
#ifndef __STM32_SPI_FLASH_H_
#define __STM32_SPI_FLASH_H_

#define RT_FLASH_SPI_BUS_NAME "spi2"

//#define log_d(...)                     rt_kprintf(__VA_ARGS__);rt_kprintf("\n")


const struct fal_partition *flash_system;
const struct fal_partition *flash_sample;
const struct fal_partition *flash_log;


#endif
