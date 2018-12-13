#include <rthw.h>
#include <rtdevice.h>
#include <math.h>
#include "SenseData.h"
#include "WorkTask.h"
#include "Uart_Screen.h"
#include "bsp_StepMotor.h"
#include "dc_motor.h"
#include "usb_hid.h"

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
static rt_thread_t work1 = RT_NULL;
static rt_thread_t work2 = RT_NULL;
static rt_thread_t work3 = RT_NULL;
static rt_thread_t work4 = RT_NULL;
/* ��ʱ���Ŀ��ƿ� */
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
/* �ص����� */
void motor_cb(rt_uint8_t motor_ch)
{
	 rt_uint8_t motor_event[4] = {EVENT_MOTOR_1, EVENT_MOTOR_2, EVENT_MOTOR_3, EVENT_MOTOR_4};
    /* �����¼� */
    rt_event_send(&work_event, motor_event[motor_ch]);
}

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
/* ��ʱ����ʱ���� (1000ms) */
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
	_rev = StepMotor_AxisMoveRel(motor, position, acceltime, deceltime, TargetSpeed);
	
	if(0 == _rev)
	{
		/* �����¼� */
		rt_event_recv(&work_event, event_temp, RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR, RT_WAITING_FOREVER, RT_NULL);		
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

	set_current_position(motor[_motor_id-1], 0); //��������������λ��
	set_min_position(motor[_motor_id-1], 0);     //����������С����λ��
	motor_set_end_indicate(motor[_motor_id-1], RT_NULL);

	return 0;
}
/**
 *	��ӡ����������(ʵ����λС��ת��)
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
 *	�����������
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
 *������������
 *
 *_ch : ͨ��ѡ��
 */
void sample_task(rt_uint8_t _ch)
{
	sample_param_t *psample_param[4] = {&sample_param_1, &sample_param_2,
											&sample_param_3, &sample_param_4};
	rt_uint8_t _work_event[4] = {EVENT_CHANNEL_1, EVENT_CHANNEL_2, EVENT_CHANNEL_3, EVENT_CHANNEL_4};
	struct light_handle_t *p_light[4] = {&h_light_1, &h_light_2, &h_light_3, &h_light_4};
	rt_uint16_t LCD_info = SAMPLE1_INFO + (_ch-1) * 32;
	char str[4][25] = {0};

	/* ����5s */
	switch_blender(_ch-1, 1);
	rt_thread_delay(5000);
	switch_blender(_ch-1, 0);
	/* ��ԡ 295s */
	cnt[_ch-1] = psample_param[_ch-1]->heat_time;
	ScreenDisICON(SAMPLE1_PROG_IOC + _ch - 1, 1); //��ʾ������
	rt_event_recv(&work_event, _work_event[_ch-1],
				  RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR, RT_WAITING_FOREVER, RT_NULL);
	/* ���չ�ֵ */
	cnt[_ch-1] = psample_param[_ch-1]->read0_time;
	/* ����R2 */
	StepMotor_AxisMoveRel_sync(_ch, 5300, 150, 150, 10000);
	rt_thread_delay(50);
	StepMotor_AxisMoveRel_sync(_ch, 1, 150, 150, 10000);
	/* ����5s */
	switch_blender(_ch-1, 1);
	rt_thread_delay(5000);
	switch_blender(_ch-1, 0);
	/* ��ֵ8min */
	p_light[_ch-1]->pA_count = RT_NULL;
	cnt[_ch-1] = psample_param[_ch-1]->read_time;   //��ʼ�ź�
	//send_start_windos(_ch-1, 1);
	en_sense_light(_ch-1, 1);
	rt_event_recv(&work_event, _work_event[_ch-1], RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR, RT_WAITING_FOREVER, RT_NULL);
	en_sense_light(_ch-1, 0);
	rt_kprintf("\n%d��������\n", _ch);
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
	rt_sprintf(str[_ch-1], "������ɣ������");
	ScreenSendCommand(WRITE_82, LCD_info, (rt_uint8_t*)str, sizeof(str));
}
/**
 *  ��������
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
			rt_sprintf(str, "�ޱ�������Լ��ٲ���");
			ScreenSendCommand(WRITE_82, LCD_info, (rt_uint8_t*)str, sizeof(str));
			*cup_en[_ch] = 0;
			return RT_ERROR;
		}
	}
	/* ���õ��ֹͣ�ص����� */
	motor_set_end_indicate(motor[_ch], pmotor_cb[_ch]);
	sample_param[_ch]->surplus_time = sample_param[_ch]->heat_time +      \
											sample_param[_ch]->read_time;
	sample_param[_ch]->total_time = sample_param[_ch]->surplus_time;
	sample_param[_ch]->total_time_1_5 = sample_param[_ch]->total_time / 5;
	rt_sprintf(str, "���ڼ���С���������");
	ScreenSendCommand(WRITE_82, LCD_info, (rt_uint8_t*)str, sizeof(str));
	*cup_en[_ch] = 0;
	count[_ch] = 0;

	return RT_EOK;
}
/*
 * �������
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
	ScreenDisICON(SAMPLE1_SWITCH_IOC + _ch, 0); //��ʾ��ʼ
	*sample_status[_ch] = FLOW_READY;
}
/*
 * ��������ʵ��
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
 * @brief  �������񴴽�
 *  
 * @param  _ch ��0 ~ 3
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
		 /* ������ʱ�� */
		timer = rt_timer_create("timer", /* ��ʱ�������� timer */
								 timeout, /* ��ʱʱ�ص��Ĵ����� */
								 RT_NULL, /* ��ʱ��������ڲ��� */
								 1000, 	  /* ��ʱ���ȣ���OS TickΪ��λ����1000��OS Tick */
								 RT_TIMER_FLAG_PERIODIC); /* �����Զ�ʱ�� */

		/* ������ʱ�� */
		if (timer != RT_NULL) rt_timer_start(timer);
	}
	task_thread_para[_ch] = _type;
	*hth[_ch] = rt_thread_create(workname[_ch],            //�߳����ơ�
								 work[_ch],      		   //�߳���ں�����
								 &task_thread_para[_ch],   //�߳���ڲ�����
								 CHANNEL_STACK_SIZE,       //�߳�ջ��С��
								 CHANNEL_PRIORITY,         //�߳����ȼ���
								 CHANNEL_TIMESLICE);       //ʱ��ƬTick��

	if(*hth[_ch] == RT_NULL)            //�ж��߳��Ƿ񴴽��ɹ���
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
 * ����ȡ��
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
	/* ��ʼ���¼����� */
	rt_event_init(&work_event, "work_event", RT_IPC_FLAG_FIFO);
	//open_usb_hid();

	return RT_EOK;
}
INIT_APP_EXPORT(worktask_init);
