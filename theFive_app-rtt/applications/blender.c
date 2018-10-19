#include "blender.h"

rt_device_t blender_dev;

/**
 *	_ch : 通道
 *	_ctl：0-->en
 *		  1-->dis
 */
rt_err_t switch_blender(rt_uint8_t _ch, rt_uint8_t _ctl)
{
	struct rt_pwm_configuration _configuration;

	RT_ASSERT(blender_dev);
	_configuration.channel = _ch;
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

	if( rt_device_write(blender_dev, _ch, &pulse, sizeof(rt_uint32_t)) != sizeof(rt_uint32_t))
	{
		rt_kprintf("write pwm channel %d: faild! \n", _ch);
		result = -RT_ERROR;
		goto _exit;
	}

_exit:
	return result;
}
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
