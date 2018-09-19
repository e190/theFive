/*
 * File      : main.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2015-07-29     Arda.Fu      first implementation
 */
#include <rtthread.h>
#include <board.h>
#include "RunLED.h"
#include "Uart_Screen.h"
#include "AnalogToDigital.h"

ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t thread_UartScreen_stack[UartScreen_STACK_SIZE];
struct rt_thread thread_UartScreen;

int main(void)
{
	rt_thread_t tid1;

	tid1 = rt_thread_create("RunLED",                 //�߳����ơ�
							 Function_RunLED,         //�߳���ں�����
							 RT_NULL,                 //�߳���ڲ�����							
							 RunLED_STACK_SIZE,       //�߳�ջ��С��
							 RunLED_PRIORITY,         //�߳����ȼ���
							 RunLED_TIMESLICE);       //ʱ��ƬTick��
							
	if(tid1 != RT_NULL)            //�ж��߳��Ƿ񴴽��ɹ���
	{
		rt_thread_startup(tid1);   //�̴߳����ɹ��������̡߳�
	}
	
	rt_thread_init(	&thread_UartScreen,
					"UartScreen",                 //�߳����ơ�
					 Function_UartScreen,         //�߳���ں�����
					 RT_NULL,                     //�߳���ڲ�����
					 thread_UartScreen_stack,
					 UartScreen_STACK_SIZE,  	  //�߳�ջ��С��
					 UartScreen_PRIORITY,         //�߳����ȼ���
					 UartScreen_TIMESLICE         //ʱ��ƬTick��
	);
	rt_thread_startup(&thread_UartScreen);
	
	tid1 = rt_thread_create("AnalogToDigital",                 //�߳����ơ�
							 Function_AnalogToDigital,         //�߳���ں�����
							 RT_NULL,                 //�߳���ڲ�����
							 AnalogToDigital_STACK_SIZE,       //�߳�ջ��С��
							 AnalogToDigital_PRIORITY,         //�߳����ȼ���
							 AnalogToDigital_TIMESLICE);       //ʱ��ƬTick��
							
	if(tid1 != RT_NULL)            //�ж��߳��Ƿ񴴽��ɹ���
	{
		rt_thread_startup(tid1);   //�̴߳����ɹ��������̡߳�
	}
    return 0;
}
