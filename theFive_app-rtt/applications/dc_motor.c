/*
 * File      : dc_motor.c
 *
 *	直流电机应用：搅拌电机和开关门电机
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-10-26     Sniper      first implementation
 */
#include <rtdevice.h>
#include <board.h>
#include "dc_motor.h"

rt_device_t blender_dev;
static DoorStatus door_1, door_2;
static DoorStatus door_old[2] = {DOOR_OPEN, DOOR_OPEN};

/**
 *	获取杯位状态
 *	w_door = 0 -->左门
 *	w_door = 1 -->右门
 *
 *	return : 1 --> open
 *			 0 --> close
 */
rt_uint8_t get_door_status(rt_uint8_t w_door)
{
	return door_old[w_door];
}
/**
 *
 *	w_door = 0 -->左门
 *	w_door = 1 -->右门
 *	_ctl = 0 -->关
 *	_ctl = 1 -->开
 */
rt_err_t door_start(rt_uint8_t w_door, DoorStatus _ctl)
{
	DoorStatus *door[2] = {&door_1, &door_2};
	RT_ASSERT(w_door < 2);
	RT_ASSERT(_ctl < 2);
	rt_uint8_t do_value = w_door<<4 | _ctl;

	if(_ctl == *door[w_door])
	{
		rt_kprintf("DOOR same status :%x\n", do_value);
		return RT_ERROR;
	}
	//ScreenDisICON(RDOOR_ICO, 0);ScreenDisICON(LDOOR_ICO, 1);
	switch (do_value)
	{
	case 0x00: //左门关
		*door[0] = DOOR_CLOSE;
		HAL_GPIO_WritePin(GPIOF, GPIO_PIN_8 | GPIO_PIN_9, GPIO_PIN_SET);
		break;
	case 0x01: //左门开
		*door[0] = DOOR_OPEN;
		HAL_GPIO_WritePin(GPIOF, GPIO_PIN_8 | GPIO_PIN_7, GPIO_PIN_SET);
		break;
	case 0x10: //右门关
		*door[1] = DOOR_CLOSE;
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOF, GPIO_PIN_10, GPIO_PIN_SET);
		break;
	case 0x11: //右门开
		*door[1] = DOOR_OPEN;
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0 | GPIO_PIN_1, GPIO_PIN_SET);
		break;
	}

	return RT_EOK;
}
/**
 *
 *	_ch = 0 -->左门
 *	_ch = 1 -->右门
 */
rt_err_t door_stop(rt_uint8_t _ch)
{
	RT_ASSERT(_ch < 2);
	if(0 == _ch)
	{
		HAL_GPIO_WritePin(GPIOF,
						  GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9,
						  GPIO_PIN_RESET);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0 | GPIO_PIN_1, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOF, GPIO_PIN_10, GPIO_PIN_RESET);
	}

	return 0;
}
/**
 *	开关门扫描检测
 */
int door_sense_scan(void)
{
	DoorStatus *door[2] = {&door_1, &door_2};
	static rt_uint8_t times[2];
	rt_int8_t d_value[2] = {door_old[0] - door_1, door_old[1] - door_2};
	rt_uint16_t gpio_pin[2] = {GPIO_PIN_4, GPIO_PIN_6};

	for(rt_uint8_t i = 0;i < 2;i++)
	{
		if(d_value[i] > 0)  // 关门
		{
			if(0 == HAL_GPIO_ReadPin(GPIOF, gpio_pin[i]))
			{
				door_stop(i);
				door_old[i] = *door[i];
			}
		}
		else if(d_value[i] < 0)
		{
			if(++times[i] > 80)
			{
				times[i] = 0;
				door_stop(i);
				door_old[i] = *door[i];
			}
		}
	}
	return 0;
}
/**
 * 搅拌电机开关
 *	_ch : 通道
 *	_ctl：0-->dis
 *		  1-->en
 */
rt_err_t switch_blender(rt_uint8_t _ch, rt_uint8_t _ctl)
{
	struct rt_pwm_configuration _configuration;

	RT_ASSERT(blender_dev);
	_configuration.channel = 3 - _ch;
	if(0 == _ctl)
		return rt_device_control(blender_dev, PWM_CMD_DISABLE, &_configuration);
	else
		return rt_device_control(blender_dev, PWM_CMD_ENABLE, &_configuration);
}
/**
 *	_ch : 通道
 *	percent：0-->10
 */
int set_blender_duty(rt_uint8_t _ch, rt_uint8_t percent)
{
	rt_err_t result = RT_EOK;
	rt_uint32_t pulse = 0;

	if(percent<0 || percent > 10)
	{
		rt_kprintf("set pwm duty %d: faild! \n", percent);
		result = -RT_ERROR;
		goto _exit;
	}
	pulse = 1000000 * (10 - percent) ;
	RT_ASSERT(blender_dev);

	if( rt_device_write(blender_dev, 3-_ch, &pulse, sizeof(rt_uint32_t)) != sizeof(rt_uint32_t))
	{
		rt_kprintf("write pwm channel %d: faild! \n", 3-_ch);
		result = -RT_ERROR;
		goto _exit;
	}

_exit:
	return result;
}
/* 开关门 */
int door_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
	/*
		door_1_en：GPIO_C_0。
		door_1_1：GPIO_C_1。
		door_1_2：GPIO_F_10。
		door_1_sense:GPIO_F_4

		door_2_1：GPIO_F_9。
		door_2_2：GPIO_F_7。
		door_2_en：GPIO_F_8。
		door_2_sense:GPIO_F_6
	*/
	GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_8, GPIO_PIN_RESET);

	GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_6;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

	door_1 = DOOR_OPEN;
	door_2 = DOOR_OPEN;

	return 0;
}
INIT_APP_EXPORT(door_init);
/**
 *	搅拌电机初始化
 */
int blender_init(void)
{
	rt_err_t result = RT_EOK;
	struct rt_pwm_configuration configuration;

	blender_dev = rt_device_find(blender_name);
	if(!blender_dev)
	{
		rt_kprintf("%s not found!\n", blender_name);
		result = -RT_ERROR;
		goto _exit;
	}
	result = rt_device_open(blender_dev, RT_DEVICE_FLAG_RDWR);
	if(result != RT_EOK)
	{
		rt_kprintf("open %s faild! \n", blender_name);
		result = -RT_EIO;
		goto _exit;
	}

	configuration.period = 10000000;    // 10e9 / period = frequency
	configuration.pulse = configuration.period / 5;
	for(rt_uint8_t i=0;i<4;i++)
	{
		configuration.channel = i;
		if( rt_device_control(blender_dev, PWM_CMD_SET, &configuration) != RT_EOK )
		{
			rt_kprintf("control PWM_CMD_SET channel %d: faild! \n", configuration.channel);
			result = -RT_ERROR;
			goto _exit;
		}
	}

	//rt_device_control(blender_dev, PWM_CMD_ENABLE, &configuration);

	_exit:
		return result;
}
INIT_APP_EXPORT(blender_init);
