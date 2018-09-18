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

	tid1 = rt_thread_create("RunLED",                 //线程名称。
							 Function_RunLED,         //线程入口函数。
							 RT_NULL,                 //线程入口参数。							
							 RunLED_STACK_SIZE,       //线程栈大小。
							 RunLED_PRIORITY,         //线程优先级。
							 RunLED_TIMESLICE);       //时间片Tick。
							
	if(tid1 != RT_NULL)            //判断线程是否创建成功。
	{
		rt_thread_startup(tid1);   //线程创建成功，启动线程。
	}
	
	rt_thread_init(	&thread_UartScreen,
					"UartScreen",                 //线程名称。
					 Function_UartScreen,         //线程入口函数。
					 RT_NULL,                     //线程入口参数。
					 thread_UartScreen_stack,
					 UartScreen_STACK_SIZE,  	  //线程栈大小。
					 UartScreen_PRIORITY,         //线程优先级。
					 UartScreen_TIMESLICE         //时间片Tick。
	);
	rt_thread_startup(&thread_UartScreen);
	
	tid1 = rt_thread_create("AnalogToDigital",                 //线程名称。
							 Function_AnalogToDigital,         //线程入口函数。
							 RT_NULL,                 //线程入口参数。
							 AnalogToDigital_STACK_SIZE,       //线程栈大小。
							 AnalogToDigital_PRIORITY,         //线程优先级。
							 AnalogToDigital_TIMESLICE);       //时间片Tick。
							
	if(tid1 != RT_NULL)            //判断线程是否创建成功。
	{
		rt_thread_startup(tid1);   //线程创建成功，启动线程。
	}
    return 0;
}
