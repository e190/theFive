#include <rthw.h>
#include <rtdevice.h>
#include <math.h>
#include "SenseData.h"
#include "WorkTask.h"
#include "Uart_Screen.h"
#include "bsp_StepMotor.h"
#include "dc_motor.h"
#include "usb_hid.h"

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
static rt_thread_t work1 = RT_NULL;
static rt_thread_t work2 = RT_NULL;
static rt_thread_t work3 = RT_NULL;
static rt_thread_t work4 = RT_NULL;
/* 定时器的控制块 */
static rt_timer_t timer;
static rt_uint8_t task_thread_para[4] = {0};

static int cnt[4] = {-1, -1, -1, -1};
typedef void (*pwork_t)(void *parameter);
typedef void (*pmotor_cb_t)(void);

sample_param_t sample_param_1 = {
								0,
								10,
								DEFAULT_HEAT_TIME,
								DEFAULT_READ_TIME,
								DEFAULT_A1_TIME,
								DEFAULT_A2_TIME
								};
sample_param_t sample_param_2 = {
								0,
								10,
								DEFAULT_HEAT_TIME,
								DEFAULT_READ_TIME,
								DEFAULT_A1_TIME,
								DEFAULT_A2_TIME
								};
sample_param_t sample_param_3 = {
								0,
								10,
								DEFAULT_HEAT_TIME,
								DEFAULT_READ_TIME,
								DEFAULT_A1_TIME,
								DEFAULT_A2_TIME
								};
sample_param_t sample_param_4 = {
								0,
								10,
								DEFAULT_HEAT_TIME,
								DEFAULT_READ_TIME,
								DEFAULT_A1_TIME,
								DEFAULT_A2_TIME
								};
/* 回调函数 */
void motor_cb(rt_uint8_t motor_ch)
{
	 rt_uint8_t motor_event[4] = {EVENT_MOTOR_1, EVENT_MOTOR_2, EVENT_MOTOR_3, EVENT_MOTOR_4};
    /* 发送事件 */
    rt_event_send(&work_event, motor_event[motor_ch]);
}

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
/* 定时器超时函数 (1000ms) */
static void timeout(void *parameter)
{
	sample_param_t* sample_param[4] = {&sample_param_1, &sample_param_2,
										&sample_param_3, &sample_param_4};
	rt_uint8_t _work_event[4] = {EVENT_CHANNEL_1, EVENT_CHANNEL_2, EVENT_CHANNEL_3, EVENT_CHANNEL_4};
	rt_uint8_t *en_read_light[4] = {&switch_config.en_Light_1, &switch_config.en_Light_2,  \
									&switch_config.en_Light_3, &switch_config.en_Light_4};
	struct light_handle_t *ppLight[4] = {&h_light_1, &h_light_2, &h_light_3, &h_light_4};
	rt_uint16_t total_time;

	for(rt_uint8_t i = 0; i<4; i++)
	{
	    if (cnt[i] > 0)    
		{
			if(sample_param[i]->a1_time == (sample_param[i]->read0_time - cnt[i] + 9)
					&& *en_read_light[i])
			{
				ppLight[i]->a0_count = 10;
				ppLight[i]->pA_count = &ppLight[i]->a0_count;
				ppLight[i]->pA_ave_1 = &ppLight[i]->ave_a0_1;
				ppLight[i]->pA_ave_2 = &ppLight[i]->ave_a0_2;
			}
			if(sample_param[i]->a1_time == (sample_param[i]->read_time - cnt[i] + 9)
					&& *en_read_light[i])
			{
				ppLight[i]->a1_count = 10;
				ppLight[i]->pA_count = &ppLight[i]->a1_count;
				ppLight[i]->pA_ave_1 = &ppLight[i]->ave_a1_1;
				ppLight[i]->pA_ave_2 = &ppLight[i]->ave_a2_2;
			}
			if(sample_param[i]->a2_time == (sample_param[i]->read_time - cnt[i] + 9)
					&& *en_read_light[i])
			{
				ppLight[i]->a2_count = 10;
				ppLight[i]->pA_count = &ppLight[i]->a2_count;
				ppLight[i]->pA_ave_1 = &ppLight[i]->ave_a2_1;
				ppLight[i]->pA_ave_2 = &ppLight[i]->ave_a2_2;
			}
			sample_param[i]->surplus_time--;
			total_time = sample_param[i]->total_time - sample_param[i]->surplus_time;
			if (total_time == sample_param[i]->total_time_1_5 * 2)
				ScreenDisICON(SAMPLE1_PROG_IOC + i, 2);
			else if (total_time == sample_param[i]->total_time_1_5 * 3)
				ScreenDisICON(SAMPLE1_PROG_IOC + i, 3);
			else if (total_time == sample_param[i]->total_time_1_5 * 4)
				ScreenDisICON(SAMPLE1_PROG_IOC + i, 4);
			else if(total_time == sample_param[i]->total_time)
				ScreenDisICON(SAMPLE1_PROG_IOC + i, 5);

			cnt[i]--;
		}
		else if(0 == cnt[i])
		{
			rt_event_send(&work_event, _work_event[i]);
			cnt[i]--;
		}
	}		
}

static void work_timer_delete(void)
{
	if(status_config.flow1_status == FLOW_READY && status_config.flow2_status == FLOW_READY
		&& status_config.flow3_status == FLOW_READY && status_config.flow4_status == FLOW_READY)
	{
		rt_timer_stop(timer);
		rt_timer_delete(timer);
		timer = RT_NULL;
	}
}
/**
 * @brief  电机启动-同步
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
	_rev = StepMotor_AxisMoveRel(motor, position, acceltime, deceltime, TargetSpeed);
	
	if(0 == _rev)
	{
		/* 接收事件 */
		rt_event_recv(&work_event, event_temp, RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR, RT_WAITING_FOREVER, RT_NULL);		
	}
	else
		rt_kprintf("motor error %d\n",_rev);
	
	return 0;
}
/**
 * 步进电机回零操作
 */
int stepmotor_backzero(rt_uint8_t _motor_id)
{
	rt_uint8_t _ret = 0;
	MotorHandle_t* motor[4] = {&Motor1, &Motor2, &Motor3, &Motor4};
	pmotor_cb_t _end_cb[4] = {motor_1_cb, motor_2_cb, motor_3_cb, motor_4_cb,};

	motor_set_end_indicate(motor[_motor_id-1], _end_cb[_motor_id-1]);
	_ret = StepMotor_AxisMoveRel_sync(_motor_id, 0, 150, 150, 10000);
	if(_ret != 0)
		return _ret;
	rt_thread_delay(10);
	_ret = StepMotor_AxisMoveRel_sync(_motor_id, -100, 0, 0, 1000);
	rt_thread_delay(10);
	_ret = StepMotor_AxisMoveRel_sync(_motor_id, 0, 0, 0, 1000);
	rt_thread_delay(10);
	_ret = StepMotor_AxisMoveRel_sync(_motor_id, -5300, 150, 150, 10000);
	if(_ret != 0)
		return _ret;

	set_current_position(motor[_motor_id-1], 0); //重新设置零坐标位置
	set_min_position(motor[_motor_id-1], 0);     //重新设置最小坐标位置
	motor_set_end_indicate(motor[_motor_id-1], RT_NULL);

	return 0;
}
/**
 *	打印浮点型数据(实现四位小数转换)
 */
void float_print(float f_data)
{
	char str[10] = {0}; // destination string
	rt_int16_t d, f;

	d = (rt_int16_t) f_data * 10000; // Decimal precision: 4 digits
	f = ( f_data * 10000 ) - d; // Decimal precision: 4 digits
	if(f_data < 0)
		rt_sprintf (str, "-%d.%04d", (rt_uint16_t)f_data, abs(f) );
	else
		rt_sprintf (str, "%d.%04d", (rt_int16_t)f_data, abs(f) ); // Decimal precision: 4 digits
	rt_kprintf("%s\n", str);
}
/**
 *	计算样本结果
 */
static float sample_calculate(rt_uint16_t _a1, rt_uint16_t _a2)
{

	float _d_value = (float)_a2 / (float)_a1;
	rt_kprintf("_d_value: ");
	float_print(_d_value);
	float _result = 0;//log10(_d_value);
	rt_kprintf("_result: ");
	//float_print(_result);

	return _result;
}
/**
 *样本测试流程
 *
 *_ch : 通道选择
 */
void sample_task(rt_uint8_t _ch)
{
	sample_param_t *psample_param[4] = {&sample_param_1, &sample_param_2,
											&sample_param_3, &sample_param_4};
	rt_uint8_t _work_event[4] = {EVENT_CHANNEL_1, EVENT_CHANNEL_2, EVENT_CHANNEL_3, EVENT_CHANNEL_4};
	struct light_handle_t *p_light[4] = {&h_light_1, &h_light_2, &h_light_3, &h_light_4};
	rt_uint16_t LCD_info = SAMPLE1_INFO + (_ch-1) * 32;
	char str[4][25] = {0};

	/* 搅拌5s */
	switch_blender(_ch-1, 1);
	rt_thread_delay(5000);
	switch_blender(_ch-1, 0);
	/* 温浴 295s */
	cnt[_ch-1] = psample_param[_ch-1]->heat_time;
	ScreenDisICON(SAMPLE1_PROG_IOC + _ch - 1, 1); //显示进度条
	rt_event_recv(&work_event, _work_event[_ch-1],
				  RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR, RT_WAITING_FOREVER, RT_NULL);
	/* 读闭光值 */
	cnt[_ch-1] = psample_param[_ch-1]->read0_time;
	/* 加入R2 */
	StepMotor_AxisMoveRel_sync(_ch, 5300, 150, 150, 10000);
	rt_thread_delay(50);
	StepMotor_AxisMoveRel_sync(_ch, 1, 150, 150, 10000);
	/* 搅拌5s */
	switch_blender(_ch-1, 1);
	rt_thread_delay(5000);
	switch_blender(_ch-1, 0);
	/* 读值8min */
	p_light[_ch-1]->pA_count = RT_NULL;
	cnt[_ch-1] = psample_param[_ch-1]->read_time;   //开始信号
	//send_start_windos(_ch-1, 1);
	en_sense_light(_ch-1, 1);
	rt_event_recv(&work_event, _work_event[_ch-1], RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR, RT_WAITING_FOREVER, RT_NULL);
	en_sense_light(_ch-1, 0);
	rt_kprintf("\n%d输出结果：\n", _ch);
	rt_kprintf("A1_1: %d\n", p_light[_ch-1]->ave_a1_1);
	rt_kprintf("A1_2: %d\n", p_light[_ch-1]->ave_a1_2);

	rt_kprintf("A2_1: %d\n", p_light[_ch-1]->ave_a2_1);
	rt_kprintf("A2_2: %d\n", p_light[_ch-1]->ave_a2_2);
	float _log = sample_calculate(p_light[_ch-1]->ave_a1_1, p_light[_ch-1]->ave_a2_1);
	//send_result_windos(_ch-1, p_light[_ch-1]->ave_a1_1, p_light[_ch-1]->ave_a2_1, _log);
	p_light[_ch-1]->ave_a1_1 = 0;
	p_light[_ch-1]->ave_a1_2 = 0;
	p_light[_ch-1]->ave_a2_1 = 0;
	p_light[_ch-1]->ave_a2_2 = 0;
	rt_sprintf(str[_ch-1], "测试完成，请继续");
	ScreenSendCommand(WRITE_82, LCD_info, (rt_uint8_t*)str, sizeof(str));
}
/**
 *  测试流程
 *
 *_ch : 通道选择
 */
static void test_task(rt_uint8_t _ch)
{
	StepMotor_AxisMoveRel_sync(_ch, 3000, 150, 150, 10000);
	rt_thread_delay(50);
	StepMotor_AxisMoveRel_sync(_ch, 1, 150, 150, 10000);
}
/*
 * 任务初始化
 */
rt_uint8_t channel_init(rt_uint8_t _ch)
{
	static rt_uint32_t count[4] = {0};
	MotorHandle_t* motor[4] = {&Motor1, &Motor2, &Motor3, &Motor4};
	pmotor_cb_t pmotor_cb[4] = {motor_1_cb, motor_2_cb, motor_3_cb, motor_4_cb};
	rt_uint8_t* cup_en[4] = {&switch_config.en_cup_1, &switch_config.en_cup_2,
									&switch_config.en_cup_3, &switch_config.en_cup_4};
	sample_param_t* sample_param[4] = {&sample_param_1, &sample_param_2,
											&sample_param_3, &sample_param_4};
	rt_uint16_t LCD_info = SAMPLE1_INFO + _ch * 32;
	char str[25] = {0};

	*cup_en[_ch] = 1;
	if(wait_door(_ch/2, DOOR_CLOSE) != RT_EOK)
	{
		return RT_ERROR;
	}
	count[_ch] = 0;
	while(0 == get_cup_status(_ch))
	{
		count[_ch]++;
		rt_thread_delay(100);
		if(count[_ch] == 3)
		{
			rt_sprintf(str, "无杯，请放试剂再测试");
			ScreenSendCommand(WRITE_82, LCD_info, (rt_uint8_t*)str, sizeof(str));
			*cup_en[_ch] = 0;
			return RT_ERROR;
		}
	}
	/* 设置电机停止回调函数 */
	motor_set_end_indicate(motor[_ch], pmotor_cb[_ch]);
	sample_param[_ch]->surplus_time = sample_param[_ch]->heat_time +      \
											sample_param[_ch]->read_time;
	sample_param[_ch]->total_time = sample_param[_ch]->surplus_time;
	sample_param[_ch]->total_time_1_5 = sample_param[_ch]->total_time / 5;
	rt_sprintf(str, "正在检测中·····");
	ScreenSendCommand(WRITE_82, LCD_info, (rt_uint8_t*)str, sizeof(str));
	*cup_en[_ch] = 0;
	count[_ch] = 0;

	return RT_EOK;
}
/*
 * 任务结束
 */
void channel_end(rt_uint8_t _ch)
{
	rt_thread_t* ppwork[4] = {&work1, &work2, &work3, &work4};
	MotorHandle_t* motor[4] = {&Motor1, &Motor2, &Motor3, &Motor4};
	rt_uint8_t* sample_status[4] = {&status_config.flow1_status, &status_config.flow2_status,
										&status_config.flow3_status, &status_config.flow4_status};

	ppwork[_ch] = RT_NULL;
	cnt[_ch] = -1;
	en_sense_light(_ch, 0);
	motor_set_end_indicate(motor[_ch], RT_NULL);
	switch_blender(_ch, 0);
	ScreenDisICON(SAMPLE1_SWITCH_IOC + _ch, 0); //显示开始
	*sample_status[_ch] = FLOW_READY;
}
/*
 * 任务运行实体
 */
void Function_Channel_1(void* parameter)
{
	rt_uint8_t type = *(rt_uint8_t*)parameter;

	switch (type)
	{
	case 1:
		if(channel_init(0) == RT_EOK)
			sample_task(1);
		channel_end(0);
		break;
	case 2:
		stepmotor_backzero(1);
		break;
	default:
		break;
	}
}
void Function_Channel_2(void* parameter)
{
	rt_uint8_t type = *(rt_uint8_t*)parameter;

	switch (type)
	{
	case 1:
		if(channel_init(1) == RT_EOK)
			sample_task(2);
		channel_end(1);
		break;
	case 2:
		stepmotor_backzero(2);
		break;
	default:
		break;
	}
}
void Function_Channel_3(void* parameter)
{
	rt_uint8_t type = *(rt_uint8_t*)parameter;

	switch (type)
	{
	case 1:
		if(channel_init(2) == RT_EOK)
			sample_task(3);
		channel_end(2);
		break;
	case 2:

		stepmotor_backzero(3);
		break;
	default:
		break;
	}

}
void Function_Channel_4(void* parameter)
{	
	rt_uint8_t type = *(rt_uint8_t*)parameter;

	switch (type)
	{
	case 1:
		rt_kprintf("work4 start\n");
		if(channel_init(3) == RT_EOK)
			sample_task(4);
		channel_end(3);
		rt_kprintf("work4 done\n");
		break;
	case 2:
		stepmotor_backzero(4);
		break;
	default:
		break;
	}

}


/**
 * @brief  工作任务创建
 *  
 * @param  _ch ：0 ~ 3
 *
 * @return
 */
int work_create(const rt_uint8_t _ch, rt_uint8_t _type)
{
	int result = RT_EOK;
	char *workname[4] = {"Cnl_1", "Cnl_2", "Cnl_3", "Cnl_4"};
	pwork_t work[4] = {Function_Channel_1, Function_Channel_2, Function_Channel_3, Function_Channel_4};
	rt_thread_t *hth[4] = {&work1, &work2, &work3, &work4};
	
	if(timer == RT_NULL)
	{
		 /* 创建定时器 */
		timer = rt_timer_create("timer", /* 定时器名字是 timer */
								 timeout, /* 超时时回调的处理函数 */
								 RT_NULL, /* 超时函数的入口参数 */
								 1000, 	  /* 定时长度，以OS Tick为单位，即1000个OS Tick */
								 RT_TIMER_FLAG_PERIODIC); /* 周期性定时器 */

		/* 启动定时器 */
		if (timer != RT_NULL) rt_timer_start(timer);
	}
	task_thread_para[_ch] = _type;
	*hth[_ch] = rt_thread_create(workname[_ch],            //线程名称。
								 work[_ch],      		   //线程入口函数。
								 &task_thread_para[_ch],   //线程入口参数。
								 CHANNEL_STACK_SIZE,       //线程栈大小。
								 CHANNEL_PRIORITY,         //线程优先级。
								 CHANNEL_TIMESLICE);       //时间片Tick。

	if(*hth[_ch] == RT_NULL)            //判断线程是否创建成功。
	{
		result = -RT_ERROR;
	}
	else
	{
		rt_kprintf("addr %d: %x\n", _ch, *hth[_ch]);
		rt_thread_startup(*hth[_ch]);
	}
	
	return result; 
}
/*
 * 工作取消
 */
rt_uint8_t work_cancel(const rt_uint8_t _ch)
{
	rt_thread_t *hth[4] = {&work1, &work2, &work3, &work4};
	rt_uint8_t _rev;
	rt_kprintf("delete addr %d: %x\n", _ch, *hth[_ch]);
	work_timer_delete();
	_rev = rt_thread_suspend(*hth[_ch]);
	_rev = rt_thread_delete(*hth[_ch]);
	channel_end(_ch);

	return _rev;
}

int worktask_init(void)
{
	/* 初始化事件对象 */
	rt_event_init(&work_event, "work_event", RT_IPC_FLAG_FIFO);
	//open_usb_hid();

	return RT_EOK;
}
INIT_APP_EXPORT(worktask_init);
