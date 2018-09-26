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
#include <cm_backtrace.h>
#include "SenseData.h"
#include "RunLED.h"
#include "Uart_Screen.h"

ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t thread_RunLED_stack[RunLED_STACK_SIZE];
static rt_uint8_t thread_UartScreen_stack[UartScreen_STACK_SIZE];
static rt_uint8_t thread_SenseData_stack[SenseData_STACK_SIZE];

struct rt_thread thread_RunLED, thread_UartScreen, thread_SenseData;

static void rtt_user_assert_hook(const char* ex, const char* func, rt_size_t line);
static rt_err_t exception_hook(void *context);

int main(void)
{
    /* 设置硬件异常钩子 */
    rt_hw_exception_install(exception_hook);

    /* 设置RTT断言钩子 */
    rt_assert_set_hook(rtt_user_assert_hook);

    /* CmBacktrace 组件初始化 */
    cm_backtrace_init("theFive", HARDWARE_VERSION, SOFTWARE_VERSION);

	rt_thread_init( &thread_RunLED,
					"RunLED",                 //线程名称。
					 Function_RunLED,         //线程入口函数。
					 RT_NULL,                 //线程入口参数。
					 thread_RunLED_stack,
					 RunLED_STACK_SIZE,       //线程栈大小。
					 RunLED_PRIORITY,         //线程优先级。
					 RunLED_TIMESLICE);       //时间片Tick。

	rt_thread_startup(&thread_RunLED);   //线程创建成功，启动线程。

	
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
	
	rt_thread_init(  &thread_SenseData,
					 "SenseData",                //线程名称。
					 Function_SenseData,         //线程入口函数。
					 RT_NULL,                    //线程入口参数。
					 thread_SenseData_stack,
					 SenseData_STACK_SIZE,       //线程栈大小。
					 SenseData_PRIORITY,         //线程优先级。
					 SenseData_TIMESLICE);       //时间片Tick。
							
	rt_thread_startup(&thread_SenseData);   //线程创建成功，启动线程。

    return 0;
}

static rt_err_t exception_hook(void *context) {
    extern long list_thread(void);
    uint8_t _continue = 1;

    rt_enter_critical();

#ifdef RT_USING_FINSH
    list_thread();
#endif

    cm_backtrace_fault(*((uint32_t *)(cmb_get_sp() + sizeof(uint32_t) * 8)), cmb_get_sp() + sizeof(uint32_t) * 9);
    while (_continue == 1);

    return RT_EOK;
}

static void rtt_user_assert_hook(const char* ex, const char* func, rt_size_t line) {
    rt_enter_critical();

    rt_kprintf("rtt", "(%s) has assert failed at %s:%ld.", ex, func, line);
    cm_backtrace_assert(cmb_get_sp());

    while(1);
}
