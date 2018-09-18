#ifndef __RUNLED_H__
#define __RUNLED_H__

#include <rtthread.h>
#include <drivers/pin.h>

#define RunLED_PRIORITY   12       //线程优先级
#define RunLED_STACK_SIZE 1024      //线程栈大小
#define RunLED_TIMESLICE  10       //时间片Tick


#define RunLED_gpio     117    //定义Pin：GPIO_D_3。
#define LED1_gpio     	53
#define LED2_gpio     	54
#define LED3_gpio    	0
#define LED4_gpio     	0

#define Rdoor_en_gpio     		26
#define Rdoormotor1_gpio     	27
#define Rdoormotor2_gpio     	22
#define Ldoor_en_gpio     		20
#define Ldoormotor1_gpio     	19
#define Ldoormotor2_gpio     	21

#define blender1_gpio     	0
#define blender2_gpio     	0
#define blender3_gpio     	0
#define blender4_gpio      46	

extern void Function_RunLED(void* parameter);
#endif
