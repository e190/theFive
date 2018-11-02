#ifndef __SAVEFLASH_H__
#define __SAVEFLASH_H__
#include "flash_cfg.h"
#include "WorkTask.h"

struct flash_para_t{
	rt_uint32_t bytes_per_sector;
	rt_uint32_t sector_count;
	/* ENV start address in flash */
	rt_uint32_t data_start_addr;
	rt_uint32_t data_end_addr;
	/* current using data section address */
	rt_uint32_t cur_using_data_bak;
	rt_uint16_t size_per_index;  //һ����������Ĵ�С
	rt_uint8_t init_ok;
};

struct mix_door_para_t
{
	rt_uint8_t mix_potency_1;	//��������   ��1~10��
	rt_uint8_t mix_potency_2;
	rt_uint8_t mix_potency_3;
	rt_uint8_t mix_potency_4;
	rt_uint16_t door_range; 	//���ŷ���   ��50~80��
};

struct flash_heat_para_t
{
	rt_uint32_t CycleTime;
	float 		iSetVal;             //�趨ֵ
	float 		uKP_Coe;             //����ϵ��
	float 		uKI_Coe;             //���ֳ���
	float 		uKD_Coe;             //΢�ֳ���
};
// �洢����
struct sample_data_t{
	char data[500];
};

// ϵͳ����
struct system_para_t{
	struct mix_door_para_t mix_door_para;
	sample_param_t sample_para_1;
	sample_param_t sample_para_2;
	sample_param_t sample_para_3;
	sample_param_t sample_para_4;
	struct flash_heat_para_t heat_para_1;
	struct flash_heat_para_t heat_para_2;
	struct flash_heat_para_t heat_para_3;
	struct flash_heat_para_t heat_para_4;
};

EfErrCode load_flash(void);
void flash_clean(void);
void flash_test_demo(rt_uint8_t num);
void flash_read_demo(rt_uint8_t num);
#endif
