#ifndef __RUNLED_H__
#define __RUNLED_H__

#define SYS_PRIORITY   12       //线程优先级
#define SYS_STACK_SIZE 1024     //线程栈大小
#define SYS_TIMESLICE  10       //时间片Tick

extern void Function_sys_monitor(void* parameter);
#endif
