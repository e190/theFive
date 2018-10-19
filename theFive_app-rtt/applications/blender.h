#ifndef __BLENDER_H__
#define __BLENDER_H__
#include <rtthread.h>
#include <rtdevice.h>

#define blender_name   	"pwm4"

rt_err_t switch_blender(rt_uint8_t _ch,rt_uint8_t _ctl);
int set_blender_duty(rt_uint8_t _ch, rt_uint8_t percent);

#endif
