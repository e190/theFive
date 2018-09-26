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
    /* ����Ӳ���쳣���� */
    rt_hw_exception_install(exception_hook);

    /* ����RTT���Թ��� */
    rt_assert_set_hook(rtt_user_assert_hook);

    /* CmBacktrace �����ʼ�� */
    cm_backtrace_init("theFive", HARDWARE_VERSION, SOFTWARE_VERSION);

	rt_thread_init( &thread_RunLED,
					"RunLED",                 //�߳����ơ�
					 Function_RunLED,         //�߳���ں�����
					 RT_NULL,                 //�߳���ڲ�����
					 thread_RunLED_stack,
					 RunLED_STACK_SIZE,       //�߳�ջ��С��
					 RunLED_PRIORITY,         //�߳����ȼ���
					 RunLED_TIMESLICE);       //ʱ��ƬTick��

	rt_thread_startup(&thread_RunLED);   //�̴߳����ɹ��������̡߳�

	
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
	
	rt_thread_init(  &thread_SenseData,
					 "SenseData",                //�߳����ơ�
					 Function_SenseData,         //�߳���ں�����
					 RT_NULL,                    //�߳���ڲ�����
					 thread_SenseData_stack,
					 SenseData_STACK_SIZE,       //�߳�ջ��С��
					 SenseData_PRIORITY,         //�߳����ȼ���
					 SenseData_TIMESLICE);       //ʱ��ƬTick��
							
	rt_thread_startup(&thread_SenseData);   //�̴߳����ɹ��������̡߳�

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
