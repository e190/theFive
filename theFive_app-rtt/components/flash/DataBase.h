
#ifndef __SAVEFLASH_H__
#define __SAVEFLASH_H__
#include <rthw.h>
#include <rtthread.h>
#include "flash_cfg.h"

struct flash_para_t{
	rt_uint32_t bytes_per_sector;
	rt_uint32_t sector_count;
	/* ENV start address in flash */
	uint32_t flash_start_addr;
	/* current using data section address */
	uint32_t cur_using_data_bak;
	uint32_t data_detail_addr;
};

struct mix_door_time_t
{
	rt_uint8_t mix_time_1;	//搅拌时间   单位：s
	rt_uint8_t mix_time_2;
	rt_uint8_t mix_time_3;
	rt_uint8_t mix_time_4;
	rt_uint16_t door_time_1; //开关门时间   单位：ms
	rt_uint16_t door_time_2;
	rt_uint16_t door_time_3;
	rt_uint16_t door_time_4;
};

struct system_para_t{

};

EfErrCode load_flash(void);
void flash_test_demo(void);

#endif
