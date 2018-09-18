#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include <rtthread.h>
#include <rtdevice.h>

/*test_pwm pwm1 500 ---> 50% 100Hz*/
rt_err_t test_pwm(int argc, char** argv)
{
	rt_err_t result = RT_EOK;
	rt_device_t dev;
	int channel = 3;
	rt_uint16_t b = 0;
	struct rt_pwm_configuration configuration;
	
	dev = rt_device_find(argv[1]);
	if(!dev)
	{
		rt_kprintf("%s not found!\n", argv[1]);
		result = -RT_ERROR;
		goto _exit;
	}
	
	result = rt_device_open(dev, RT_DEVICE_FLAG_RDWR);
	if(result != RT_EOK)
	{
		rt_kprintf("open %s faild! \n", argv[1]);
		result = -RT_EIO;
		goto _exit;
	}
	
	rt_kprintf("test pwm: %s\n", argv[1]);

	rt_uint32_t pulse,pulse2;
	channel = 3;
	configuration.channel = channel;
	configuration.period = 10000000;    // 10e9 / period = frequency 
	configuration.pulse = configuration.period / 2;
	rt_kprintf("\ntest pwm set channel: %d, period: %d, pulse: %d\n", channel, configuration.period, configuration.pulse); 
	
	if( rt_device_control(dev, PWM_CMD_SET, &configuration) != RT_EOK )
	{
		rt_kprintf("control PWM_CMD_SET channel %d: faild! \n", channel);
		result = -RT_ERROR;
		goto _exit;
	}
//	if( rt_device_read(dev, channel, &pulse2, sizeof(rt_uint32_t)) != sizeof(rt_uint32_t))
//	{
//		rt_kprintf("read pwm channel %d: faild! \n", channel);
//		result = -RT_ERROR;
//		goto _exit;
//	}
	b = atoi(argv[2]);
	pulse2 = 10000000 / 1000;
	pulse = pulse2 * b;
	rt_kprintf("pwm pulse %d \n", pulse);
	if( rt_device_write(dev, channel, &pulse, sizeof(rt_uint32_t)) != sizeof(rt_uint32_t))
	{
		rt_kprintf("write pwm channel %d: faild! \n", channel);
		result = -RT_ERROR;
		goto _exit;
	}	

	rt_pwm_enable(channel);

_exit:
	return result;
}

#include <finsh.h>

MSH_CMD_EXPORT(test_pwm, test pwm);
