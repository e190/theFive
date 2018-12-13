#include <rtthread.h>
#include <board.h>
#include <cm_backtrace.h>
#include "SysMonitor.h"
#include "SenseData.h"
#include "Uart_Screen.h"
#include "Heat_PID.h"
#include "dc_motor.h"

//#define LOG_TAG  "main"
//#define DBG_ENABLE
//#define DBG_SECTION_NAME               "MAIN"
//#define DBG_LEVEL                      DBG_INFO
//#define DBG_COLOR
//#include <rtdbg.h>

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
    /* 设置硬件异常钩子 */
    rt_hw_exception_install(exception_hook);

    /* 设置RTT断言钩子 */
    rt_assert_set_hook(rtt_user_assert_hook);

    /* CmBacktrace 组件初始化 */
    cm_backtrace_init("theFive_app-rtt", HARDWARE_VERSION, SOFTWARE_VERSION);

	rt_thread_init( &thread_sys_monitor,
					"sys_monitor",                 //线程名称。
					Function_sys_monitor,     	//线程入口函数。
					 RT_NULL,                	//线程入口参数。
					 thread_sys_monitor_stack,
					 SYS_STACK_SIZE,       		//线程栈大小。
					 SYS_PRIORITY,         		//线程优先级。
					 SYS_TIMESLICE);       		//时间片Tick。

	rt_thread_startup(&thread_sys_monitor);   //线程创建成功，启动线程。
	
	rt_thread_init(	&thread_UartScreen,
					"UartScreen",                 //线程名称。
					 Function_UartScreen,         //线程入口函数。
					 RT_NULL,                     //线程入口参数。
					 thread_UartScreen_stack,
					 UartScreen_STACK_SIZE,  	  //线程栈大小。
					 UartScreen_PRIORITY,         //线程优先级。
					 UartScreen_TIMESLICE);       //时间片Tick。

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

	rt_thread_delay(1000);

	ScreenPage(74);   //跳到初始化页面
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
//	work_create(0, 1);	//创建任务
//	work_create(1, 1);	//创建任务
	switch_config.temp_dis = 1;
	all_heat_start_stop(1);
	/* 初始化时间 */
	real_time_init();
	/* 初始化flash及系统参数 */

    return 0;
}
/**
 *	初始化RTC
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

    LOG_E("rtt (%s) has assert failed at %s:%ld.", ex, func, line);
    cm_backtrace_assert(cmb_get_sp());

    while(1);
}
