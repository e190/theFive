#ifndef __ANALOGTODIGITAL_H__
#define __ANALOGTODIGITAL_H__

#include <rthw.h>
#include <rtthread.h>
#include <stdio.h>
#include <string.h>

#define AnalogToDigital_PRIORITY 11         //线程优先级
#define AnalogToDigital_STACK_SIZE 512      //线程栈大小
#define AnalogToDigital_TIMESLICE 10        //时间片Tick

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
