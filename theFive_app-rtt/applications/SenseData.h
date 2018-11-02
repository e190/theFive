#ifndef __SENSEDATA_H__
#define __SENSEDATA_H__

#include <rthw.h>
#include <rtthread.h>
#include <stdio.h>
#include <string.h>

#define SenseData_PRIORITY 10         //�߳����ȼ�
#define SenseData_STACK_SIZE 1024      //�߳�ջ��С
#define SenseData_TIMESLICE 50        //ʱ��ƬTick

#define CupSense1_gpio     12
#define CupSense2_gpio     7
#define CupSense3_gpio     5
#define CupSense4_gpio     4

struct light_handle_t
{
	rt_uint8_t channel;			// ͨ��

	rt_uint16_t dis_addr_1;		//340��ַ
	rt_uint16_t dis_addr_2;		//740��ַ
	rt_uint16_t dis_icon_1;
	rt_uint16_t dis_icon_2;
	rt_int8_t a1_status;
	rt_int8_t a2_status;
	rt_uint32_t ave_a1_1;
	rt_uint32_t ave_a1_2;
	rt_uint32_t ave_a2_1;
	rt_uint32_t ave_a2_2;
	rt_uint8_t abs_value_1;
	rt_uint8_t abs_value_2;
};

/* ����ʹ�ܿ����ܻ� */
struct switch_config_t
{
	rt_uint8_t sample_start;
	rt_uint8_t sample_stop;
	rt_uint8_t en_Light_1;
	rt_uint8_t en_Light_2;
	rt_uint8_t en_Light_3;
	rt_uint8_t en_Light_4;
	rt_uint8_t en_Temp_1;//�¶ȼ��ʹ��
	rt_uint8_t en_Temp_2;
	rt_uint8_t en_Temp_3;
	rt_uint8_t en_Temp_4;
	rt_uint8_t en_Heat_1;//����ʹ��
	rt_uint8_t en_Heat_2;
	rt_uint8_t en_Heat_3;
	rt_uint8_t en_Heat_4;
	rt_uint8_t en_sample_1;//��������ʹ��
	rt_uint8_t en_sample_2;
	rt_uint8_t en_sample_3;
	rt_uint8_t en_sample_4;
	rt_uint8_t en_cup_1; //��λ���ʹ��
	rt_uint8_t en_cup_2;
	rt_uint8_t en_cup_3;
	rt_uint8_t en_cup_4;
};
typedef enum {
	FLOW_READY = 0,
	FLOW_ADD,
	FLOW_RUN,

}flow_status;
/* ״̬ */
struct status_config_t
{
	flow_status flow1_status; //����״̬
	flow_status flow2_status;
	flow_status flow3_status;
	flow_status flow4_status;
	rt_uint8_t temp1_status; //�¶�״̬
	rt_uint8_t temp2_status;
	rt_uint8_t temp3_status;
	rt_uint8_t temp4_status;
	rt_uint8_t cup1_status; //��λ���״̬
	rt_uint8_t cup2_status;
	rt_uint8_t cup3_status;
	rt_uint8_t cup4_status;
};
extern struct light_handle_t h_light_1, h_light_2, h_light_3, h_light_4;
extern struct switch_config_t switch_config;
extern struct status_config_t status_config;
rt_uint8_t get_cup_status(rt_uint8_t _ch);
rt_uint8_t Get_AdsData(rt_uint8_t _config, rt_uint32_t* out_data);
extern void Function_SenseData(void* parameter);

#endif
