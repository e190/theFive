#ifndef __DC_MOTOR_H__
#define __DC_MOTOR_H__
#include <rtthread.h>

#define blender_name   	"pwm4"

typedef enum {
	DOOR_CLOSE,
	DOOR_RUN,
	DOOR_OPEN
}DoorStatus;

rt_uint8_t get_door_status(rt_uint8_t door_ch);
rt_uint8_t wait_door(rt_uint8_t door_ch, DoorStatus _status);
rt_err_t door_start(rt_uint8_t door_ch, DoorStatus _ctl);
rt_err_t switch_blender(rt_uint8_t _ch,rt_uint8_t _ctl);
int set_blender_duty(rt_uint8_t _ch, rt_uint8_t percent);
int door_sense_scan(void);

void test_door(rt_uint8_t w_door, DoorStatus _ctl);

#endif
