#include <rtthread.h>
#include <board.h>
#include <cm_backtrace.h>
#include "SysMonitor.h"
#include "SenseData.h"
#include "Uart_Screen.h"
#include "Heat_PID.h"
#include "dc_motor.h"
#include "WorkTask.h"
#include "DataBase.h"

ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t thread_sys_monitor_stack[SYS_STACK_SIZE];
static rt_uint8_t thread_UartScreen_stack[UartScreen_STACK_SIZE];
static rt_uint8_t thread_SenseData_stack[SenseData_STACK_SIZE];

struct rt_thread thread_sys_monitor, thread_UartScreen, thread_SenseData;

static rt_err_t real_time_init(void);
static void rtt_user_assert_hook(const char* ex, const char* func, rt_size_t line);
static rt_err_t exception_hook(void *context);

int main(void)
{
    /* ����Ӳ���쳣���� */
    rt_hw_exception_install(exception_hook);

    /* ����RTT���Թ��� */
    rt_assert_set_hook(rtt_user_assert_hook);

    /* CmBacktrace �����ʼ�� */
    cm_backtrace_init("theFive_app-rtt", HARDWARE_VERSION, SOFTWARE_VERSION);

    /* ��������ʼ�� */
	uart_open(RT_SCREEN_DEVICE_NAME);

	rt_thread_init( &thread_sys_monitor,
					"sys_monitor",                 //�߳����ơ�
					Function_sys_monitor,     	//�߳���ں�����
					 RT_NULL,                	//�߳���ڲ�����
					 thread_sys_monitor_stack,
					 SYS_STACK_SIZE,       		//�߳�ջ��С��
					 SYS_PRIORITY,         		//�߳����ȼ���
					 SYS_TIMESLICE);       		//ʱ��ƬTick��

	rt_thread_startup(&thread_sys_monitor);   //�̴߳����ɹ��������̡߳�
	
	rt_thread_init(	&thread_UartScreen,
					"UartScreen",                 //�߳����ơ�
					 Function_UartScreen,         //�߳���ں�����
					 RT_NULL,                     //�߳���ڲ�����
					 thread_UartScreen_stack,
					 UartScreen_STACK_SIZE,  	  //�߳�ջ��С��
					 UartScreen_PRIORITY,         //�߳����ȼ���
					 UartScreen_TIMESLICE);       //ʱ��ƬTick��
	
	rt_thread_init(  &thread_SenseData,
					 "SenseData",                //�߳����ơ�
					 Function_SenseData,         //�߳���ں�����
					 RT_NULL,                    //�߳���ڲ�����
					 thread_SenseData_stack,
					 SenseData_STACK_SIZE,       //�߳�ջ��С��
					 SenseData_PRIORITY,         //�߳����ȼ���
					 SenseData_TIMESLICE);       //ʱ��ƬTick��
							
	rt_thread_startup(&thread_SenseData);   //�̴߳����ɹ��������̡߳�
	rt_pin_write(BUZZER_PIN, 1);
	rt_thread_delay(1000);
	rt_pin_write(BUZZER_PIN, 0);

	ScreenPage(74);   //������ʼ��ҳ��
	door_start(0, DOOR_CLOSE);
	door_start(1, DOOR_CLOSE);
	if(wait_door(0, DOOR_CLOSE) != RT_EOK)
	{
		rt_kprintf("door wait timeout\n");
		return 1;
	}
	if(wait_door(1, DOOR_CLOSE) != RT_EOK)
	{
		rt_kprintf("door wait timeout\n");
		return 1;
	}

	switch_config.temp_dis = 1;

	/* ��ʼ��flash��ϵͳ���� */
	load_flash();

	worktask_init();
	head_system_init();

//	work_create(0, motorZero);	//��������
//	work_create(1, motorZero);
//	work_create(2, motorZero);
//	work_create(3, motorZero);
	if(wait_all_work())
	{
		rt_kprintf("work or motor init failed\n");
		return 1;
	}
	//all_heat_start_stop(1);

	rt_thread_startup(&thread_UartScreen);
	/* ��ʼ��ʱ�� */
	real_time_init();

	rt_kprintf("theFive initialize success. Firmware version is %s", SOFTWARE_VERSION);

    return 0;
}
/**
 *	��ʼ��RTC
 */
static rt_err_t real_time_init(void)
{
	rt_uint8_t _cmd = 0x06;
    time_t old, new;
    /* output current time */
    old = time(RT_NULL);
    rt_kprintf("%s", ctime(&old));

	ScreenSendCommand(READ_81, 0x20, &_cmd, 1);
	rt_thread_delay(1000);
	new = time(RT_NULL);
    rt_kprintf("%s", ctime(&new));

    return RT_EOK;
}
static rt_err_t exception_hook(void *context) {
    extern long list_thread(void);
    rt_uint8_t _continue = 1;

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

    LOG_E("rtt (%s) has assert failed at %s:%ld.", ex, func, line);
    cm_backtrace_assert(cmb_get_sp());

    while(1);
}
