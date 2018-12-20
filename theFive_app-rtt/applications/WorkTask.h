#ifndef __WORKTASK_H__
#define __WORKTASK_H__
#include <rtthread.h>

#define CHANNEL_PRIORITY 10         //线程优先级
#define CHANNEL_STACK_SIZE 1024      //线程栈大小
#define CHANNEL_TIMESLICE 10        //时间片Tick

#define EN_CHANNEL_1	0x01
#define EN_CHANNEL_2	0x02
#define EN_CHANNEL_3	0x04
#define EN_CHANNEL_4	0x08

#define DIS_CHANNEL_1	0x10
#define DIS_CHANNEL_2	0x20
#define DIS_CHANNEL_3	0x40
#define DIS_CHANNEL_4	0x80

#if 1
#define DEFAULT_PARAM           \
    {                           \
        .read0_time = 10,       \
		.heat_time = 5,         \
        .read_time = 320,       \
        .a1_time = 150,       	\
        .a2_time = 300,       	\
    }
#else
#define DEFAULT_PARAM           \
    {                           \
        .read0_time = 10,       \
		.heat_time = 300,       \
        .read_time = 320,       \
        .a1_time = 150,       	\
        .a2_time = 300,       	\
    }
#endif

enum workType{
	smaple = 1,
	motorZero,
	work_test,
};

typedef struct 
{
	rt_uint8_t mix_time;
	rt_uint8_t read0_time;
    rt_uint16_t heat_time;
    rt_uint16_t read_time;
    rt_uint16_t a1_time;
    rt_uint16_t a2_time;
	
    rt_uint16_t total_time;   //总时间
	rt_uint16_t total_time_1_5;   //总时间的1/5
	rt_uint16_t surplus_time;
}sample_param_t;

struct task_thread_para_t
{
	rt_uint8_t ch_1;
	rt_uint8_t ch_2;
	rt_uint8_t ch_3;
	rt_uint8_t ch_4;
};

extern sample_param_t sample_param_1;
extern sample_param_t sample_param_2;
extern sample_param_t sample_param_3;
extern sample_param_t sample_param_4;

void float_print(float f_data);

int worktask_init(void);
int stepmotor_backzero(rt_uint8_t _motor_id);
int work_create(const rt_uint8_t _ch, rt_uint8_t _type);
rt_uint8_t work_cancel(const rt_uint8_t _ch);
rt_uint8_t work_is_ready(const rt_uint8_t _ch);
rt_uint8_t all_work_is_ready(void);
rt_uint8_t wait_all_work(void);

#endif
