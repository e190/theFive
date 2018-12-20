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
#if defined(RT_USING_SFUD)

#include <drv_spi.h>
#include "spi_flash.h"
#include "spi_flash_sfud.h"
#include "flash_cfg.h"

#if defined(PKG_USING_FAL)
#include "fal.h"
#endif

//#define FLASH_DEBUG
#define LOG_TAG             "flash.init"
#include <flash_log.h>

rt_device_t w25q128 = RT_NULL;

const struct fal_partition *flash_system = RT_NULL;
const struct fal_partition *flash_sample = RT_NULL;
const struct fal_partition *flash_log= RT_NULL;

static rt_uint32_t flash_cache[ERASE_MIN_SIZE / 4] = { 0 };

//EfErrCode get_flash_geometry(struct rt_device_blk_geometry *flash_geometry)
//{
//	EfErrCode _ret = F_NO_ERR;
//	rt_err_t get_result = RT_EOK;
//
//	RT_ASSERT(flash_geometry);
//	get_result = rt_device_control(w25q128, RT_DEVICE_CTRL_BLK_GETGEOME, flash_geometry);
//	LOG_I("bytes_per_sector : %d",flash_geometry->bytes_per_sector);
//	LOG_I("sector count : %d",flash_geometry->sector_count);
//	LOG_I("block size : %d",flash_geometry->block_size);
//    if(get_result != RT_EOK) {
//    	_ret = F_DATA_INIT_FAILED;
//    }
//
//	return _ret;
//}

static void flash_port_init(void)
{
	flash_system = fal_partition_find("system");
	flash_sample = fal_partition_find("sample");
	flash_log = fal_partition_find("log");
}

int rt_nor_flash_init(void)
{
	rt_spi_flash_device_t spi_device;

    rt_hw_spi_device_attach("spi2", "norspi", GPIOA, GPIO_PIN_4);
    spi_device = rt_sfud_flash_probe("flash0", "norspi");
    if (spi_device == RT_NULL)
    {
    	LOG_E("sfud flash probe failed");
        return -RT_ERROR;
    }

    w25q128 = rt_device_find("flash0");
	if(w25q128 == RT_NULL)
		return -RT_ERROR;
	rt_device_open(w25q128, RT_DEVICE_FLAG_RDWR);

	fal_init();

	flash_port_init();

    return 0;
}

INIT_DEVICE_EXPORT(rt_nor_flash_init);

rt_uint16_t get_sector_addr(rt_uint32_t addr)
{
	rt_uint16_t sector_addr = addr / ERASE_MIN_SIZE;

	return sector_addr;
}
/**
 *	擦除flash
 *	addr ： 块地址
 *	size ： 块数量
 */
static int erase(long offset, size_t size)
{
    assert(w25q128);

    rt_uint32_t address[2]={offset/ERASE_MIN_SIZE, size/ERASE_MIN_SIZE + 1};

    if (rt_device_control(w25q128, RT_DEVICE_CTRL_BLK_ERASE, address) != SFUD_SUCCESS)
    {
    	LOG_E("erase flash failed");
        return -1;
    }
    LOG_I("erase sector addr:%d, size:%d", address[0], address[1]);

    return size;
}
static int read(long offset, uint8_t *buf, size_t size)
{
	size_t total_size = size;
    size_t readable = 0,read_size = 0;
	char *flash_cache_bak = (char *)flash_cache;

	LOG_I("read addr:%d", offset);

	do{
		rt_uint16_t sector_addr = get_sector_addr(offset);
		rt_uint16_t detail_addr = offset % ERASE_MIN_SIZE;
		if(detail_addr + total_size > ERASE_MIN_SIZE)
		{
			readable = ERASE_MIN_SIZE - detail_addr;
		}
		else
			readable = total_size;
		LOG_I("sector addr : %d,readable : %d", sector_addr, readable);
		if(rt_device_read(w25q128, sector_addr, flash_cache, 1) == 0) {
			LOG_E("read flash failed");
			return -1;
		}
		rt_memcpy(buf + read_size, flash_cache_bak + detail_addr, readable);
		read_size += readable;
		total_size -= readable;
		offset += readable;
	}
	while(total_size);

	for(rt_uint8_t i = 0;i < size;i++)
	{
#ifdef FLASH_DEBUG
		rt_kprintf("%d ", buf[i]);
#endif /* FLASH_DEBUG */
	}

    return size;
}

static int write(long offset, const uint8_t *buf, size_t size)
{
    assert(w25q128);

    size_t total_size = size;
    size_t write_size = 0, writable_size = 0;
    char *flash_cache_bak = (char *)flash_cache;

    LOG_I("write addr:%d", offset);
	do {
		rt_uint16_t sector_addr = get_sector_addr(offset);
		uint16_t detail_addr = offset % ERASE_MIN_SIZE;
		if(detail_addr != 0)
		{
			if(rt_device_read(w25q128, sector_addr, flash_cache, 1) == 0) {
				LOG_E("read flash failed");
				return -1;
			}
		}

		if(detail_addr + total_size > ERASE_MIN_SIZE)
		{
			writable_size = ERASE_MIN_SIZE - detail_addr;
		}
		else
			writable_size = total_size;
		LOG_I("sector addr : %d,writable_size : %d", sector_addr, writable_size);
		rt_memcpy(flash_cache_bak + detail_addr, buf + write_size, writable_size);
		if(rt_device_write(w25q128, sector_addr, flash_cache, 1) == 0) {
			LOG_E("write flash failed");
			return -1;
		}
		write_size += writable_size;
		offset += writable_size;
		total_size -= writable_size;
	}
	while(total_size);

	for(rt_uint8_t i = 0;i < size;i++)
	{
#ifdef FLASH_DEBUG
		rt_kprintf("%d ", buf[i]);
#endif /* FLASH_DEBUG */
	}

    return size;
}

const struct fal_flash_dev nor_flash0 = { "norflash0", 0, 16*1024*1024, 4096, {NULL, read, write, erase} };

#if defined(RT_USING_FINSH) && defined(FINSH_USING_MSH)
#include <finsh.h>

extern void flash_read_demo(rt_uint8_t num);

void read_flash(uint8_t argc, char **argv) {
	rt_uint32_t num;
	rt_uint8_t buf[10] = {0};
	num = atoi(argv[1]);

	read(num, buf, sizeof(buf));
}
MSH_CMD_EXPORT(read_flash, read flash to cache.);

void write_flash(uint8_t argc, char **argv) {
	rt_uint32_t num;
	rt_uint8_t buf[10] = {11, 22, 33, 44, 55, 66, 77, 88, 99, 0};
	num = atoi(argv[1]);

	write(num, buf, sizeof(buf));
}
MSH_CMD_EXPORT(write_flash, read flash to cache.);

void read_index(uint8_t argc, char **argv) {
	rt_uint32_t num;
	num = atoi(argv[1]);
	flash_read_demo(num);
}
MSH_CMD_EXPORT(read_index, read index from flash.);

void clean_flash(uint8_t argc, char **argv) {
	EfErrCode _ret = F_NO_ERR;
	rt_uint32_t start_time, time_cast;
	rt_uint32_t num;
	num = atoi(argv[1]);
	start_time = rt_tick_get();
	//_ret = erase(0, num); //擦除全部
	fal_partition_erase_all(flash_system);
	//fal_partition_erase_all(flash_sample);
	time_cast = rt_tick_get() - start_time;
    if (_ret != F_NO_ERR) {
    	rt_kprintf("erase flash %d is error.\n", num);
    }else
    {
    	rt_kprintf("erase flash  %d is done., total time: %d.%03dS.\n", num, time_cast / RT_TICK_PER_SECOND,
                time_cast % RT_TICK_PER_SECOND / ((RT_TICK_PER_SECOND * 1 + 999) / 1000));
    }
}
MSH_CMD_EXPORT(clean_flash, erase flash.);

#endif /* defined(RT_USING_FINSH) && defined(FINSH_USING_MSH) */

#endif /*defined(RT_USING_W25QXX) || defined(RT_USING_SFUD)*/
