#ifndef __RUNLED_H__
#define __RUNLED_H__

#define SYS_PRIORITY   12       //�߳����ȼ�
#define SYS_STACK_SIZE 1024     //�߳�ջ��С
#define SYS_TIMESLICE  10       //ʱ��ƬTick

extern void Function_sys_monitor(void* parameter);
#endif
