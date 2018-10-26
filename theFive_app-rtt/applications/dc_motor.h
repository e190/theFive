#ifndef __DC_MOTOR_H__
#define __DC_MOTOR_H__
#include <rtthread.h>

#define blender_name   	"pwm4"

typedef enum {
	DOOR_CLOSE,
	DOOR_OPEN,
	DOOR_RUN,
}DoorStatus;

rt_err_t switch_blender(rt_uint8_t _ch,rt_uint8_t _ctl);
int set_blender_duty(rt_uint8_t _ch, rt_uint8_t percent);

#endif
