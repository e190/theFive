/**
 *
 * This all Backup Area Flash storage index. All used flash area configure is under here.
 *	Storage Size ---- 128M bit(16M bytes)
 * |----------------------------|
 * |      system section      	|   system section size
 * |----------------------------|
 * |      data section        	|   data section size
 * |----------------------------|
 * |      Saved log area        |   Log area size
 * |----------------------------|
 * |(IAP)Downloaded application |   IAP already downloaded application, unfixed size
 * |----------------------------|
 *
 */
#ifndef __SAVEFLASH_H__
#define __SAVEFLASH_H__
#include <rthw.h>
#include <rtthread.h>

#define TOTAL_SIZE		0x1000000	//flash总容量16mb (0~0x1000000)
#define SYS_SIZE		0x80
#define DATA_SIZE		0x100
#define LOG_SIZE		0x100
#define IAP_SIZE		0x100

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
