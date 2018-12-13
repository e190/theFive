/*
 * File      : gpio.c
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
#include <rtthread.h>
#include "spi_flash_init.h"
#if defined(RT_USING_W25QXX) || defined(RT_USING_SFUD)
    #include <drv_spi.h>
#ifdef RT_USING_W25QXX
    #include "spi_flash_w25qxx.h"
#elif defined(RT_USING_SFUD)
    #include "string.h"
    #include "spi_flash.h"
    #include "spi_flash_sfud.h"
    rt_device_t w25q128 = RT_NULL; 
#endif

int rt_nor_flash_init(void)
{
	rt_spi_flash_device_t spi_device;
    //stm32_spi_bus_attach_device(RT_FLASH_CS_PIN, RT_FLASH_SPI_BUS_NAME, "norspi");
    rt_hw_spi_device_attach("spi2", "norspi", GPIOA, GPIO_PIN_4);
#ifdef RT_USING_W25QXX
    return w25qxx_init("flash0", "norspi");
#elif defined(RT_USING_SFUD)
    spi_device = rt_sfud_flash_probe("flash0", "norspi");
    if (spi_device == RT_NULL)
    {
        return -RT_ERROR;
    }
		
    w25q128 = rt_device_find("flash0");
	if(w25q128 == RT_NULL)
		return -RT_ERROR;
	rt_device_open(w25q128, RT_DEVICE_FLAG_RDWR);
		
    return 0;
#endif
}
INIT_DEVICE_EXPORT(rt_nor_flash_init);
#endif
