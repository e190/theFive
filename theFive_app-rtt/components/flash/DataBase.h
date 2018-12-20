#ifndef __SAVEFLASH_H__
#define __SAVEFLASH_H__
#include "flash_cfg.h"
#include "WorkTask.h"
#include "Heat_PID.h"

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

// 存储数据
struct sample_data_t{
	char data[500];
};

struct mix_door_para_t
{
	rt_uint8_t mix_potency;	//搅拌力度   （1~10）
	rt_uint8_t door_range; 	//开门幅度   （50~80）
};

#define  SAMPLE_PARA_POS  0
#define  HEAT_PARA_POS   	  sizeof(sample_param_t) * 4
#define  MIX_DOOR_PARA_POS    HEAT_PARA_POS + sizeof(struct PID_Value) * 4

// 系统参数
struct system_para_t{
	sample_param_t sample_para_1;
	sample_param_t sample_para_2;
	sample_param_t sample_para_3;
	sample_param_t sample_para_4;
	struct PID_Value heat_para_1;
	struct PID_Value heat_para_2;
	struct PID_Value heat_para_3;
	struct PID_Value heat_para_4;
	struct mix_door_para_t mix_door_para;
};

void set_system_para_cache(rt_off_t pos, const void *_buffer, rt_size_t _size);
void get_system_para_cache(rt_off_t pos, void *_buffer, rt_size_t _size);
EfErrCode fresh_system_para(void);
EfErrCode get_system_para(void);
int load_flash(void);
void flash_clean(void);
void flash_test_demo(rt_uint8_t num);
void flash_read_demo(rt_uint8_t num);

#endif
