#include <rthw.h>
#include <rtdevice.h>
#include <SenseData.h>
#include "WorkTask.h"
#include "Uart_Screen.h"
#include "bsp_StepMotor.h"
#include "RunLED.h"

/* �����¼���־ */
#define EVENT_MOTOR_1	(1 << 0)
#define EVENT_MOTOR_2 	(1 << 1)
#define EVENT_MOTOR_3 	(1 << 2)
#define EVENT_MOTOR_4 	(1 << 3)

#define EVENT_CHANNEL_1 (1 << 4)
#define EVENT_CHANNEL_2 (1 << 5)
#define EVENT_CHANNEL_3 (1 << 6)
#define EVENT_CHANNEL_4 (1 << 7)

/* �¼����ƿ� */
static struct rt_event work_event;
rt_thread_t work1 = RT_NULL;
rt_thread_t work2 = RT_NULL;
rt_thread_t work3 = RT_NULL;
rt_thread_t work4 = RT_NULL;
/* ��ʱ���Ŀ��ƿ� */
static rt_timer_t timer;
struct task_thread_para_t task_thread_para = {1,2,3,4};

static int cnt[4] = {-1,-1,-1,-1};
typedef void (*pwork_t)(void *parameter);
typedef void (*pworkend_t)(void);

sample_param_t sample_param_1 = {
							DEFAULT_HEAT_TIME,
							DEFAULT_READ_TIME,
							DEFAULT_A1_TIME,
							DEFAULT_A2_TIME,
							SAMPLE1_HEAT_TIME,
							SAMPLE1_READ_TIME,
							SAMPLE1_A1_TIME,
							SAMPLE1_A2_TIME
							};
sample_param_t sample_param_2 = {
							DEFAULT_HEAT_TIME,
							DEFAULT_READ_TIME,
							DEFAULT_A1_TIME,
							DEFAULT_A2_TIME,
							SAMPLE2_HEAT_TIME,
							SAMPLE2_READ_TIME,
							SAMPLE2_A1_TIME,
							SAMPLE2_A2_TIME
							};
sample_param_t sample_param_3 = {
							DEFAULT_HEAT_TIME,
							DEFAULT_READ_TIME,
							DEFAULT_A1_TIME,
							DEFAULT_A2_TIME,
							SAMPLE3_HEAT_TIME,
							SAMPLE3_READ_TIME,
							SAMPLE3_A1_TIME,
							SAMPLE3_A2_TIME
							};
sample_param_t sample_param_4 = {
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
    /* �����¼� */
    rt_event_send(&work_event, EVENT_MOTOR_1);
}
void motor_2_cb(void)
{
    /* �����¼� */
    rt_event_send(&work_event, EVENT_MOTOR_2);
}
void motor_3_cb(void)
{
    /* �����¼� */
    rt_event_send(&work_event, EVENT_MOTOR_3);
}
void motor_4_cb(void)
{
    /* �����¼� */
    rt_event_send(&work_event, EVENT_MOTOR_4);
}
/* ��ʱ����ʱ���� */
static void timeout(void *parameter)
{
	sample_param_t* sample_param[4] = {&sample_param_1, &sample_param_2,
									&sample_param_3, &sample_param_4};
	rt_uint16_t surplus_tim_LCD[4] = {SAMPLE1_SURPLUS_TIME, SAMPLE2_SURPLUS_TIME,
										SAMPLE3_SURPLUS_TIME, SAMPLE4_SURPLUS_TIME};
	for(rt_uint8_t i = 0; i<4; i++)
	{
	    if (cnt[i] > 0)    
		{
			rt_kprintf("periodic timer is timeout\n");
			if(sample_param_1.a1_time == (sample_param_1.read_time - cnt[i] + 10))
			{
				h_light_1.a1_status = 1;
			}
			if(sample_param_1.a2_time == (sample_param_1.read_time - cnt[i] + 10))
			{
				h_light_1.a2_status = 1;
			}
			sample_param[i]->surplus_time--;
			ScreenSendData_2bytes(surplus_tim_LCD[i], sample_param[i]->surplus_time);
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
	if(switch_config.en_sample_1 == 0 && switch_config.en_sample_2 == 0
		&& switch_config.en_sample_3 == 0 && switch_config.en_sample_4 == 0)
	{
		rt_timer_stop(timer);
		rt_timer_delete(timer);
		timer = RT_NULL;
	}
}
/**
 * @brief  �������-ͬ��
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
	//rt_kprintf("task start\n");
	_rev = StepMotor_AxisMoveRel(motor, position, acceltime, deceltime, TargetSpeed);
	
	if(0 == _rev)
	{
		/* �����¼� */
		rt_event_recv(&work_event, event_temp, RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR, RT_WAITING_FOREVER, RT_NULL);		
		//rt_kprintf("task done\n");
	}
	else
		rt_kprintf("motor error %d\n",_rev);
	
	return 0;
}
/**
 * ��������������
 */
int stepmotor_backzero(rt_uint8_t _motor_id)
{
	rt_uint8_t _ret = 0;
	MotorHandle_t* motor[4] = {&Motor1, &Motor2, &Motor3, &Motor4};
	pworkend_t _end_cb[4] = {motor_1_cb, motor_2_cb, motor_3_cb, motor_4_cb,};

	motor_set_end_indicate(motor[_motor_id-1], _end_cb[_motor_id-1]);
	_ret = StepMotor_AxisMoveRel_sync(_motor_id, 0, 150, 150, 10000);
	if(_ret != 0)
		return _ret;
	rt_thread_delay(10);
	_ret = StepMotor_AxisMoveRel_sync(_motor_id, -100, 0, 0, 1000);
	rt_thread_delay(10);
	_ret = StepMotor_AxisMoveRel_sync(_motor_id, 0, 0, 0, 1000);
	rt_thread_delay(10);
	_ret = StepMotor_AxisMoveRel_sync(_motor_id, -3800, 150, 150, 10000);
	if(_ret != 0)
		return _ret;

	set_current_position(motor[_motor_id-1], 0); //��������������λ��
	set_min_position(motor[_motor_id-1], 0);     //����������С����λ��
	motor_set_end_indicate(motor[_motor_id-1], RT_NULL);

	return 0;
}
void parameter_display(sample_param_t* sample_param)
{
	ScreenSendData(sample_param->heat_dis_addr, (rt_uint8_t*)&sample_param->heat_time, 2);
	ScreenSendData(sample_param->read_dis_addr, (rt_uint8_t*)&sample_param->read_time, 2);
	ScreenSendData(sample_param->a1_dis_addr, (rt_uint8_t*)&sample_param->a1_time, 2);
	ScreenSendData(sample_param->a2_dis_addr, (rt_uint8_t*)&sample_param->a2_time, 2);
}
/**
 *������������
 *
 *_ch : ͨ��ѡ��
 */
void sample_task(rt_uint8_t _ch)
{
	rt_uint8_t _blender_gpio[4] = {blender1_gpio, blender2_gpio, blender3_gpio, blender4_gpio};
	sample_param_t *psample_param[4] = {&sample_param_1, &sample_param_2, &sample_param_3, &sample_param_4};
	rt_uint8_t _work_event[4] = {EVENT_CHANNEL_1, EVENT_CHANNEL_2, EVENT_CHANNEL_3, EVENT_CHANNEL_4};

	/* ����5s */
	rt_pin_write(_blender_gpio[_ch-1], 0);
	rt_thread_delay(5000);
	rt_pin_write(_blender_gpio[_ch-1], 1);
	/* ��ԡ 295s */
	ScreenDisICON(SAMPLE4_1_ICO, 1);
	cnt[3] = psample_param[_ch-1]->heat_time;
	rt_event_recv(&work_event, _work_event[_ch-1],
				  RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR, RT_WAITING_FOREVER, RT_NULL);
	/* ����R2 */
	StepMotor_AxisMoveRel_sync(_ch, 2000, 150, 150, 10000);
	rt_thread_delay(50);
	StepMotor_AxisMoveRel_sync(_ch, 1, 150, 150, 10000);
	/* ����5s */
	rt_pin_write(_blender_gpio[_ch-1], 0);
	rt_thread_delay(5000);
	rt_pin_write(_blender_gpio[_ch-1], 1);
	/* ��ֵ8min */
	ScreenDisICON(SAMPLE4_2_ICO, 1);
	cnt[_ch-1] = psample_param[_ch-1]->read_time;
	switch_config.en_Light_1 = 1;
	rt_event_recv(&work_event, _work_event[_ch-1], RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR, RT_WAITING_FOREVER, RT_NULL);
	switch_config.en_Light_1 = 0;
}
/**
 *��������
 *
 *_ch : ͨ��ѡ��
 */
static void test_task(rt_uint8_t _ch)
{
	StepMotor_AxisMoveRel_sync(_ch, 3000, 150, 150, 10000);
	rt_thread_delay(50);
	StepMotor_AxisMoveRel_sync(_ch, 1, 150, 150, 10000);
}
/*
 * �����ʼ��
 */
void channel_1_init(void)
{
	/* ���õ��ֹͣ�ص����� */
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
	sample_param_4.surplus_time = sample_param_4.heat_time + sample_param_4.read_time;
}
/*
 * �������
 */
void channel_1_end(void)
{
	work1 = RT_NULL;
	motor_set_end_indicate(&Motor1, RT_NULL);
	ScreenDisICON(SAMPLE1_SWITCH_IOC, 0);
	switch_config.en_sample_1 = 0;
}
void channel_2_end(void)
{
	work2 = RT_NULL;
	motor_set_end_indicate(&Motor2, RT_NULL);
	ScreenDisICON(SAMPLE2_SWITCH_IOC, 0);
	switch_config.en_sample_2 = 0;
}
void channel_3_end(void)
{
	work3 = RT_NULL;
	motor_set_end_indicate(&Motor3, RT_NULL);
	ScreenDisICON(SAMPLE3_SWITCH_IOC, 0);
	switch_config.en_sample_3 = 0;
}
void channel_4_end(void)
{
	work4 = RT_NULL;
	motor_set_end_indicate(&Motor4, RT_NULL);
	cnt[3] = -1;
	rt_pin_write(blender4_gpio, 1); 
	ScreenDisICON(SAMPLE4_SWITCH_IOC, 0);
	ScreenDisICON(SAMPLE4_1_ICO, 0);
	ScreenDisICON(SAMPLE4_2_ICO, 0);
	ScreenDisICON(SAMPLE_4_ICO, 1);
	switch_config.en_sample_4 = 0;
}
/*
 * ��������ʵ��
 */
void Function_Channel_1(void* parameter)
{
	channel_1_init();
	test_task(1);
	channel_1_end();
}
void Function_Channel_2(void* parameter)
{
	channel_2_init();
	test_task(2);
	channel_2_end();
}
void Function_Channel_3(void* parameter)
{
	channel_3_init();
	test_task(3);
	channel_3_end();
}
void Function_Channel_4(void* parameter)
{	
	rt_kprintf("work4 start\n");
	channel_4_init();
	/* ����5s */
	rt_pin_write(blender4_gpio, 0); 
	rt_thread_delay(5000);
	rt_pin_write(blender4_gpio, 1); 
	/* ��ԡ 295s */
	ScreenDisICON(SAMPLE4_1_ICO, 1);
	cnt[3] = sample_param_4.heat_time;
	rt_event_recv(&work_event, EVENT_CHANNEL_4,
				  RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR, RT_WAITING_FOREVER, RT_NULL);
	/* ����R2 */
	StepMotor_AxisMoveRel_sync(4,2000,150,150,10000);
	rt_thread_delay(50);
	StepMotor_AxisMoveRel_sync(4,1,150,150,10000);	
	/* ����5s */
	rt_pin_write(blender4_gpio, 0); 
	rt_thread_delay(5000);
	rt_pin_write(blender4_gpio, 1); 
	/* ��ֵ8min */
	ScreenDisICON(SAMPLE4_2_ICO, 1);
	cnt[3] = sample_param_4.read_time;
	switch_config.en_Light_1 = 1;
	rt_event_recv(&work_event, EVENT_CHANNEL_4, RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR, RT_WAITING_FOREVER, RT_NULL);
	switch_config.en_Light_1 = 0;
	channel_4_end();
	rt_kprintf("work4 done\n");
}


/**
 * @brief  �������񴴽�
 *  
 * @param  config ��1��ʹ��ͨ��һ
 *					2��ʹ��ͨ����
 *					4��ʹ��ͨ����
 *					8��ʹ��ͨ����
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
		 /* ������ʱ�� */
		timer = rt_timer_create("timer", /* ��ʱ�������� timer */
								 timeout, /* ��ʱʱ�ص��Ĵ����� */
								 RT_NULL, /* ��ʱ��������ڲ��� */
								 1000, /* ��ʱ���ȣ���OS TickΪ��λ����10��OS Tick */
								 RT_TIMER_FLAG_PERIODIC); /* �����Զ�ʱ�� */

		/* ������ʱ�� */
		if (timer != RT_NULL) rt_timer_start(timer);
	}
	for(rt_uint8_t i = 0; i<4; i++)
	{
		if(en_confg[i] & config)
		{
			*hth[i] = rt_thread_create(workname[i],            //�߳����ơ�
									 work[i],      			   //�߳���ں�����
									 &task_thread_para.ch_1+i, //�߳���ڲ�����
									 CHANNEL_STACK_SIZE,       //�߳�ջ��С��
									 CHANNEL_PRIORITY,         //�߳����ȼ���
									 CHANNEL_TIMESLICE);       //ʱ��ƬTick��
									
			if(*hth[i] == RT_NULL)            //�ж��߳��Ƿ񴴽��ɹ���
			{
				result = -RT_ERROR;		
			}
			else
			{
				//rt_kprintf("addr: %x\n",*hth[i]);
				rt_thread_startup(*hth[i]);
			}
		}
	}
	
	return result; 
}
/*
 * ����ȡ��
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
	/* ��ʼ���¼����� */
	rt_event_init(&work_event, "work_event", RT_IPC_FLAG_FIFO);
	
	return RT_EOK;
}
INIT_APP_EXPORT(worktask_init);
