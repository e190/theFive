#ifndef __WORKTASK_H__
#define __WORKTASK_H__
#include <rtthread.h>

#define CHANNEL_PRIORITY 10         //线程优先级
#define CHANNEL_STACK_SIZE 512      //线程栈大小
#define CHANNEL_TIMESLICE 10        //时间片Tick

#define EN_CHANNEL_1	0x01
#define EN_CHANNEL_2	0x02
#define EN_CHANNEL_3	0x04
#define EN_CHANNEL_4	0x08

#define DIS_CHANNEL_1	0x10
#define DIS_CHANNEL_2	0x20
#define DIS_CHANNEL_3	0x40
#define DIS_CHANNEL_4	0x80

#define DEFAULT_HEAT_TIME	295
#define DEFAULT_READ_TIME	480 
#define DEFAULT_A1_TIME		300
#define DEFAULT_A2_TIME		480

typedef struct 
{
    rt_uint16_t heat_time;
    rt_uint16_t read_time;
    rt_uint16_t a1_time;
    rt_uint16_t a2_time;
	
	rt_uint16_t heat_dis_addr;
    rt_uint16_t read_dis_addr;
    rt_uint16_t a1_dis_addr;
    rt_uint16_t a2_dis_addr;
	
	rt_uint16_t surplus_time;
}work_param_t;

extern work_param_t work_param_1;
extern work_param_t work_param_2;
extern work_param_t work_param_3;
extern work_param_t work_param_4;

void parameter_display(work_param_t* work_param);
int worktask_init(void);
void do_worktask(void);
int work_create(const rt_uint8_t config);
void work_cancel(const rt_uint8_t config);

#endif
