#include <rthw.h>
#include <rtdevice.h>
#include "WorkTask.h"
#include "Uart_Screen.h"
#include "bsp_StepMotor.h"
#include "AnalogToDigital.h"
#include "RunLED.h"

/* 工作事件标志 */
#define EVENT_MOTOR_1	(1 << 0)
#define EVENT_MOTOR_2 	(1 << 1)
#define EVENT_MOTOR_3 	(1 << 2)
#define EVENT_MOTOR_4 	(1 << 3)

#define EVENT_CHANNEL_1 (1 << 4)
#define EVENT_CHANNEL_2 (1 << 5)
#define EVENT_CHANNEL_3 (1 << 6)
#define EVENT_CHANNEL_4 (1 << 7)

/* 事件控制块 */
static struct rt_event work_event;
rt_thread_t work1 = RT_NULL;
rt_thread_t work2 = RT_NULL;
rt_thread_t work3 = RT_NULL;
rt_thread_t work4 = RT_NULL;
/* 定时器的控制块 */
static rt_timer_t timer;

static int cnt[4] = {-1,-1,-1,-1};
typedef void (*pwork_t)(void *parameter);
typedef void (*pworkend_t)(void);

work_param_t work_param_1 = {
							DEFAULT_HEAT_TIME,
							DEFAULT_READ_TIME,
							DEFAULT_A1_TIME,
							DEFAULT_A2_TIME,
							SAMPLE1_HEAT_TIME,
							SAMPLE1_READ_TIME,
							SAMPLE1_A1_TIME,
							SAMPLE1_A2_TIME
							};
work_param_t work_param_2 = {
							DEFAULT_HEAT_TIME,
							DEFAULT_READ_TIME,
							DEFAULT_A1_TIME,
							DEFAULT_A2_TIME,
							SAMPLE2_HEAT_TIME,
							SAMPLE2_READ_TIME,
							SAMPLE2_A1_TIME,
							SAMPLE2_A2_TIME
							};
work_param_t work_param_3 = {
							DEFAULT_HEAT_TIME,
							DEFAULT_READ_TIME,
							DEFAULT_A1_TIME,
							DEFAULT_A2_TIME,
							SAMPLE3_HEAT_TIME,
							SAMPLE3_READ_TIME,
							SAMPLE3_A1_TIME,
							SAMPLE3_A2_TIME
							};
work_param_t work_param_4 = {
							DEFAULT_HEAT_TIME,
							DEFAULT_READ_TIME,
							DEFAULT_A1_TIME,
							DEFAULT_A2_TIME,
							SAMPLE4_HEAT_TIME,
							SAMPLE4_READ_TIME,
							SAMPLE4_A1_TIME,
							SAMPLE4_A2_TIME
							};

void motor_1_cb(void)
{
    /* 发送事件 */
    rt_event_send(&work_event, EVENT_MOTOR_1);
}
void motor_2_cb(void)
{
    /* 发送事件 */
    rt_event_send(&work_event, EVENT_MOTOR_2);
}
void motor_3_cb(void)
{
    /* 发送事件 */
    rt_event_send(&work_event, EVENT_MOTOR_3);
}
void motor_4_cb(void)
{
    /* 发送事件 */
    rt_event_send(&work_event, EVENT_MOTOR_4);
}
/* 定时器超时函数 */
static void timeout(void *parameter)
{
	work_param_t* work_param[4] = {&work_param_1, &work_param_2,
									&work_param_3, &work_param_4};
	rt_uint16_t surplus_tim_LCD[4] = {SAMPLE1_SURPLUS_TIME, SAMPLE2_SURPLUS_TIME,
										SAMPLE3_SURPLUS_TIME, SAMPLE4_SURPLUS_TIME};
	for(rt_uint8_t i = 0; i<4; i++)
	{
	    if (cnt[i] > 0)    
		{
			rt_kprintf("periodic timer is timeout\n");
			if(work_param_1.a1_time == (work_param_1.read_time - cnt[i] + 10))
			{
				h_light_1.a1_status = 1;
			}
			if(work_param_1.a2_time == (work_param_1.read_time - cnt[i] + 10))
			{
				h_light_1.a2_status = 1;
			}
			work_param[i]->surplus_time--;
			ScreenSendData_2bytes(surplus_tim_LCD[i], work_param[i]->surplus_time);
			cnt[i]--;
		}
		else if(0 == cnt[i])
		{
			rt_event_send(&work_event, EVENT_CHANNEL_4);
			cnt[i]--;
		}
	}		
}

static void work_timer_delete(void)
{
	if(switch_config.sample_1 == 0 && switch_config.sample_2 == 0 
		&& switch_config.sample_3 == 0 && switch_config.sample_4 == 0)
	{
		rt_timer_stop(timer);
		rt_timer_delete(timer);
		timer = RT_NULL;
	}
}
/**
 * @brief  启动-同步
 *  
 * @param 
 *
 * @return
 */
static rt_uint8_t StepMotor_AxisMoveRel_sync(rt_uint8_t motor_id, rt_int32_t position, 
											rt_uint32_t acceltime, rt_uint32_t deceltime, 
											rt_uint32_t TargetSpeed)
{
	rt_uint32_t event_temp;
	rt_uint8_t _rev = 0;
	MotorHandle_t* motor = RT_NULL;
	switch (motor_id)
    {
	case 1:
		motor = &Motor1;
		event_temp = EVENT_MOTOR_1;
		break;
	case 2:
		motor = &Motor2;
		event_temp = EVENT_MOTOR_2;
		break;
	case 3:
		motor = &Motor3;
		event_temp = EVENT_MOTOR_3;
		break;
	case 4:
		motor = &Motor4;
		event_temp = EVENT_MOTOR_4;
		break;
	default:
		rt_kprintf("motor para error\n");
		return 0;
    }
	
	rt_kprintf("task start\n");	
	_rev = StepMotor_AxisMoveRel(motor, position, acceltime, deceltime, TargetSpeed);
	
	if(0 == _rev)
	{
		/* 接收事件 */
		rt_event_recv(&work_event, event_temp, RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR, RT_WAITING_FOREVER, RT_NULL);		
		rt_kprintf("task done\n");
	}
	else
		rt_kprintf("motor error %d\n",_rev);
	
	return 0;
}

void parameter_display(work_param_t* work_param)
{
	ScreenSendData(work_param->heat_dis_addr, (rt_uint8_t*)&work_param->heat_time, 2);
	ScreenSendData(work_param->read_dis_addr, (rt_uint8_t*)&work_param->read_time, 2);
	ScreenSendData(work_param->a1_dis_addr, (rt_uint8_t*)&work_param->a1_time, 2);
	ScreenSendData(work_param->a2_dis_addr, (rt_uint8_t*)&work_param->a2_time, 2);
}
/*
 * 任务初始化
 */
void channel_1_init(void)
{
	/* 设置电机停止回调函数 */
	motor_set_end_indicate(&Motor1, motor_1_cb);
}
void channel_2_init(void)
{
	motor_set_end_indicate(&Motor2, motor_2_cb);
}
void channel_3_init(void)
{
	motor_set_end_indicate(&Motor3, motor_3_cb);
}
void channel_4_init(void)
{
	ScreenDisICON(SAMPLE_4_ICO, 0);
	motor_set_end_indicate(&Motor4, motor_4_cb);
	work_param_4.surplus_time = work_param_4.heat_time + work_param_4.read_time;
}
/*
 * 任务结束
 */
void channel_1_end(void)
{

}
void channel_2_end(void)
{

}
void channel_3_end(void)
{

}
void channel_4_end(void)
{
	work4 = RT_NULL;
	cnt[3] = -1;
	rt_pin_write(blender4_gpio, 1); 
	ScreenDisICON(SAMPLE4_SWITCH_IOC, 0);
	ScreenDisICON(SAMPLE4_1_ICO, 0);
	ScreenDisICON(SAMPLE4_2_ICO, 0);
	ScreenDisICON(SAMPLE_4_ICO, 1);
	switch_config.sample_4 = 0;
}
/*
 * 任务运行实体
 */
void Function_Channel_1(void* parameter)
{
	StepMotor_AxisMoveRel_sync(1,1000,150,150,10000);
	rt_thread_delay(50);
	StepMotor_AxisMoveRel_sync(1,1,150,150,10000);
}
void Function_Channel_2(void* parameter)
{
	StepMotor_AxisMoveRel_sync(2,1000,150,150,10000);
	rt_thread_delay(50);
	StepMotor_AxisMoveRel_sync(2,1,150,150,10000);
}
void Function_Channel_3(void* parameter)
{
	StepMotor_AxisMoveRel_sync(3,1000,150,150,10000);
	rt_thread_delay(50);
	StepMotor_AxisMoveRel_sync(3,1,150,150,10000);
}
void Function_Channel_4(void* parameter)
{	
	rt_kprintf("work4 start\n");
	channel_4_init();
	/* 搅拌5s */
	rt_pin_write(blender4_gpio, 0); 
	rt_thread_delay(5000);
	rt_pin_write(blender4_gpio, 1); 
	/* 温浴 295s */
	ScreenDisICON(SAMPLE4_1_ICO, 1);
	cnt[3] = work_param_4.heat_time;
	rt_event_recv(&work_event, EVENT_CHANNEL_4, RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR, RT_WAITING_FOREVER, RT_NULL);
	/* 加入R2 */
	StepMotor_AxisMoveRel_sync(4,2000,150,150,10000);
	rt_thread_delay(50);
	StepMotor_AxisMoveRel_sync(4,1,150,150,10000);	
	/* 搅拌5s */
	rt_pin_write(blender4_gpio, 0); 
	rt_thread_delay(5000);
	rt_pin_write(blender4_gpio, 1); 
	/* 读值8min */
	ScreenDisICON(SAMPLE4_2_ICO, 1);
	cnt[3] = work_param_4.read_time;
	switch_config.en_Light_1 = 1;
	rt_event_recv(&work_event, EVENT_CHANNEL_4, RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR, RT_WAITING_FOREVER, RT_NULL);
	switch_config.en_Light_1 = 0;
	channel_4_end();
	rt_kprintf("work4 done\n");
}


/**
 * @brief  工作任务创建
 *  
 * @param  config ：1：使能通道一
 *					2：使能通道二
 *					4：使能通道三
 *					8：使能通道四
 * @return
 */
int work_create(const rt_uint8_t config)
{
	int result = RT_EOK;
	char *workname[4] = {"Cnl_1", "Cnl_2", "Cnl_3", "Cnl_4"};
	pwork_t work[4] = {Function_Channel_1, Function_Channel_2, Function_Channel_3, Function_Channel_4};
	rt_uint8_t en_confg[4] = {EN_CHANNEL_1, EN_CHANNEL_2, EN_CHANNEL_3, EN_CHANNEL_4};
	work1 = work2 = work3 = work4 = RT_NULL;
	rt_thread_t *hth[4] = {&work1, &work2, &work3, &work4};
	
	if(timer == RT_NULL)
	{
		 /* 创建定时器 */
		timer = rt_timer_create("timer", /* 定时器名字是 timer */
								 timeout, /* 超时时回调的处理函数 */
								 RT_NULL, /* 超时函数的入口参数 */
								 1000, /* 定时长度，以OS Tick为单位，即10个OS Tick */
								 RT_TIMER_FLAG_PERIODIC); /* 周期性定时器 */

		/* 启动定时器 */
		if (timer != RT_NULL) rt_timer_start(timer);
	}
	for(rt_uint8_t i = 0; i<4; i++)
	{
		if(en_confg[i] & config)
		{
			*hth[i] = rt_thread_create(workname[i],            //线程名称。
									 work[i],      			   //线程入口函数。
									 RT_NULL,                  //线程入口参数。
									 CHANNEL_STACK_SIZE,       //线程栈大小。
									 CHANNEL_PRIORITY,         //线程优先级。
									 CHANNEL_TIMESLICE);       //时间片Tick。
									
			if(*hth[i] == RT_NULL)            //判断线程是否创建成功。
			{
				result = -RT_ERROR;		
			}
			else
			{
				rt_kprintf("addr: %x\n",*hth[i]);
				rt_thread_startup(*hth[i]);
			}
		}
	}
	
	return result; 
}
/*
 * 工作取消
 */
void work_cancel(const rt_uint8_t config)
{
	rt_thread_t *hth[4] = {&work1, &work2, &work3, &work4};
	rt_uint8_t dis_confg[4] = {DIS_CHANNEL_1, DIS_CHANNEL_2, DIS_CHANNEL_3, DIS_CHANNEL_4};
	pworkend_t workend[4] = {channel_1_end, channel_2_end, channel_3_end, channel_4_end};
	
	work_timer_delete();
	for(rt_uint8_t i=0;i<4;i++)
	{
		if(config & dis_confg[i])
		{
			rt_thread_suspend(*hth[i]);
			rt_thread_delete(*hth[i]);
			workend[i]();
		}
	}
}

int worktask_init(void)
{
	/* 初始化事件对象 */
	rt_event_init(&work_event, "work_event", RT_IPC_FLAG_FIFO);
	
	return RT_EOK;
}
INIT_APP_EXPORT(worktask_init);
