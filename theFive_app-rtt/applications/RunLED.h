#ifndef __RUNLED_H__
#define __RUNLED_H__

#include <rtthread.h>
#include <drivers/pin.h>

#define RunLED_PRIORITY   12       //�߳����ȼ�
#define RunLED_STACK_SIZE 1536      //�߳�ջ��С
#define RunLED_TIMESLICE  10       //ʱ��ƬTick

#define RunLED_gpio     117    //����Pin��GPIO_D_3��
#define buzzer_gpio     76    //����Pin��GPIO_D_15��
#define SD_IN_gpio     	80		//SD������

#define LED1_gpio     	122
#define LED2_gpio     	123
#define LED3_gpio    	125
#define LED4_gpio     	126
#define LED5_gpio     	127
#define LED6_gpio     	128
#define LED7_gpio    	129
#define LED8_gpio     	132

#define Rdoor_en_gpio     		26
#define Rdoormotor1_gpio     	27
#define Rdoormotor2_gpio     	22
#define Ldoor_en_gpio     		20
#define Ldoormotor1_gpio     	19
#define Ldoormotor2_gpio     	21

extern void Function_RunLED(void* parameter);
#endif
