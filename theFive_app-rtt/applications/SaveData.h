#ifndef __SAVEDATA_H__
#define __SAVEDATA_H__
#include <rthw.h>
#include <rtthread.h>

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

#endif
