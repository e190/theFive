#ifndef __ANALOGTODIGITAL_H__
#define __ANALOGTODIGITAL_H__

#include <rthw.h>
#include <rtthread.h>
#include <stdio.h>
#include <string.h>

#define AnalogToDigital_PRIORITY 11         //�߳����ȼ�
#define AnalogToDigital_STACK_SIZE 512      //�߳�ջ��С
#define AnalogToDigital_TIMESLICE 10        //ʱ��ƬTick

typedef struct 
{
	rt_uint8_t a1_status;
	rt_uint8_t a2_status;
	rt_uint32_t ave_a1;
	rt_uint32_t ave_a2;
	rt_uint8_t abs_value;
	
}light_handle_t;

extern light_handle_t h_light_1;

extern void Function_AnalogToDigital(void* parameter);

#endif
