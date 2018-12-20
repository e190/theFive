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
static DoorStatus door_new[2] = {0};
static DoorStatus door_old[2] = {0};
rt_err_t door_stop(rt_uint8_t _ch);
/**
 *	door_ch = 0 -->左门
 *	door_ch = 1 -->右门
 *	_ctl = 0 -->关
 *	_ctl = 1 -->开
 */
void test_door(rt_uint8_t door_ch, DoorStatus _ctl)
{
	RT_ASSERT(door_ch < 2);
	RT_ASSERT(_ctl < 2);
	rt_uint8_t do_value = door_ch<<4 | _ctl;

	switch (do_value)
	{
	case 0x00: //左门关
		HAL_GPIO_WritePin(GPIOF, GPIO_PIN_8 | GPIO_PIN_9, GPIO_PIN_SET);
		break;
	case 0x01: //左门开
		HAL_GPIO_WritePin(GPIOF, GPIO_PIN_8 | GPIO_PIN_7, GPIO_PIN_SET);
		break;
	case 0x10: //右门关
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOF, GPIO_PIN_10, GPIO_PIN_SET);
		break;
	case 0x11: //右门开
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0 | GPIO_PIN_1, GPIO_PIN_SET);
		break;
	}
	rt_thread_delay(3000);
	door_stop(door_ch);
}

/**
 *	获取杯位状态
 *	door_ch = 0 -->左门
 *	door_ch = 1 -->右门
 *
 *	return : 1 --> open
 *			 0 --> close
 */
rt_uint8_t get_door_status(rt_uint8_t door_ch)
{
	//rt_kprintf("door%d : %d\n", door_ch, door_new[door_ch]);
	return door_new[door_ch];
}
/**
 *	等待状态
 *	door_ch：通道
 *	_status：需要等待的状态
 */
rt_uint8_t wait_door(rt_uint8_t door_ch, DoorStatus _status)
{
	rt_uint32_t count[2] = {0};

	while(get_door_status(door_ch) != _status)
	{
		count[door_ch]++;
		rt_thread_delay(100);
		if(count[door_ch] == 100)
		{
			//rt_kprintf("door%d wait timeout\n", door_ch);
			return RT_ERROR;
		}
	}

	return RT_EOK;
}
/**
 *
 *	door_ch = 0 -->左门
 *	door_ch = 1 -->右门
 *	_ctl = 0 -->关
 *	_ctl = 2 -->开
 */
rt_err_t door_start(rt_uint8_t door_ch, DoorStatus _ctl)
{
	RT_ASSERT(door_ch < 2);
	RT_ASSERT(_ctl == 0 || _ctl == 2);
	rt_uint8_t do_value = door_ch<<4 | _ctl;

	if(DOOR_RUN == door_new[door_ch])
	{
		rt_kprintf("DOOR runing\n");
		return RT_ETIMEOUT;
	}

	if(_ctl == door_old[door_ch])
	{
		rt_kprintf("DOOR same status :%x\n", do_value);
		return RT_ERROR;
	}
	//ScreenDisICON(RDOOR_ICO, 0);ScreenDisICON(LDOOR_ICO, 1);
	door_new[door_ch] = DOOR_RUN;
	switch (do_value)
	{
	case 0x00: //左门关
		HAL_GPIO_WritePin(GPIOF, GPIO_PIN_8 | GPIO_PIN_9, GPIO_PIN_SET);
		break;
	case 0x02: //左门开
		HAL_GPIO_WritePin(GPIOF, GPIO_PIN_8 | GPIO_PIN_7, GPIO_PIN_SET);
		break;
	case 0x10: //右门关
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOF, GPIO_PIN_10, GPIO_PIN_SET);
		break;
	case 0x12: //右门开
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
	static rt_uint8_t times[2];
	rt_int8_t d_value[2] = {door_old[0] - door_new[0], door_old[1] - door_new[1]};
	rt_uint16_t gpio_pin[2] = {GPIO_PIN_4, GPIO_PIN_6};

	for(rt_uint8_t i = 0;i < 2;i++)
	{
		if(d_value[i] > 0)  //关门--判断传感器
		{
			if(0 == HAL_GPIO_ReadPin(GPIOF, gpio_pin[i]))
			{
				door_stop(i);
				door_new[i] = DOOR_CLOSE;
				door_old[i] = door_new[i];
			}
		}
		else if(d_value[i] < 0)//开门--计时间
		{
			if(++times[i] > 80)
			{
				times[i] = 0;
				door_stop(i);
				door_new[i] = DOOR_OPEN;
				door_old[i] = door_new[i];
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
	_configuration.channel = 4 - _ch;
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

	if( rt_device_write(blender_dev, 4 - _ch, &pulse, sizeof(rt_uint32_t)) != sizeof(rt_uint32_t))
	{
		rt_kprintf("write pwm channel %d: faild! \n", 4 -_ch);
		result = -RT_ERROR;
		goto _exit;
	}

_exit:
	return result;
}
/* 开关门GPIO初始化  */
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

	door_old[0] = DOOR_OPEN;
	door_old[1] = DOOR_OPEN;

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
	configuration.pulse = configuration.period / 10 * 7;
	for(rt_uint8_t i = 0; i < 4; i++)
	{
		configuration.channel = i + 1;
		if( rt_device_control(blender_dev, PWM_CMD_SET, &configuration) != RT_EOK )
		{
			rt_kprintf("control PWM_CMD_SET channel %d: faild! \n", configuration.channel);
			result = -RT_ERROR;
			goto _exit;
		}
	}

	//rt_device_control(blender_dev, PWM_CMD_ENABLE, &configuration);
	rt_kprintf("blender PWM init success! \n", blender_name);
	_exit:
		return result;
}
INIT_APP_EXPORT(blender_init);
