#ifndef __SAVEFLASH_H__
#define __SAVEFLASH_H__
#include "flash_cfg.h"

struct flash_para_t{
	rt_uint32_t bytes_per_sector;
	rt_uint32_t sector_count;
	/* ENV start address in flash */
	rt_uint32_t data_start_addr;
	rt_uint32_t data_end_addr;
	/* current using data section address */
	rt_uint32_t cur_using_data_bak;
	rt_uint16_t size_per_index;  //一条样本结果的大小
	rt_uint8_t init_ok;
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
struct sample_data_t{
	char data[500];
};
struct system_para_t{

};

EfErrCode load_flash(void);
void flash_clean(void);
void flash_test_demo(rt_uint8_t num);
void flash_read_demo(rt_uint8_t num);
#endif
