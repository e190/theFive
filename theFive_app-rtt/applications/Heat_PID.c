#include <stdio.h>
#include <string.h>
#include "Heat_PID.h"

struct HeatSystem_t HeatHandle_1, HeatHandle_2, HeatHandle_3, HeatHandle_4;

/**
 * @brief  计算PID
 *  	   位置式PID: 
 * @param 
 *
 * @return
 */
float pid_calculate(struct HeatSystem_t* h_heat)
{
	rt_uint8_t index = 0;
	float _temp = 0;
	PID_Value* _pid = &h_heat->PID;
	
	_pid->err = h_heat->iSetVal - h_heat->iCurVal;
	if (_pid->err>20)
	{
		index = 0;
	}
	else 
	{
		index = 1;
		_pid->integral += _pid->err;
	}

	_temp = _pid->uKP_Coe * _pid->err + 
			index * _pid->uKI_Coe * _pid->integral +
			_pid->uKD_Coe * (_pid->err - _pid->err_last);
	
	_pid->err_last = _pid->err;
	
	if(_temp > 100)
		_temp = 100;
	else if(_temp < 0)
		_temp = 0;

	return _temp;
}

void set_heat_pos()
{

}
/*
 * 初始化加热系统
 */
void heat_init(struct HeatSystem_t* h_heat)
{
	h_heat->deviceName = "pwm1";
	h_heat->PWM_channel = 3;
	h_heat->dev = rt_device_find(h_heat->deviceName);
	if(!h_heat->dev)
	{
		rt_kprintf("%s not found!\n", h_heat->deviceName);
	}
	h_heat->PID.uKP_Coe = 4;
	h_heat->PID.uKI_Coe = 0.02;
	h_heat->PID.uKD_Coe = 50;
	h_heat->PID.liEkVal[0] = 0;
	h_heat->PID.liEkVal[1] = 0;
	h_heat->PID.liEkVal[2] = 0;
	h_heat->iSetVal = 370;
	h_heat->iCurVal = 0;
	h_heat->CycleTime = 5;
}
/*
 * 设置PWM占空比
 * percent : 占空比 * 1000
 */
int set_duty(struct HeatSystem_t* h_heat, rt_uint16_t percent)
{
	rt_err_t result = RT_EOK;
	rt_uint32_t pulse = 0;
		
	pulse = (TIME_FREQUENCY / 100) * percent ;
	if( rt_device_write(h_heat->dev, h_heat->PWM_channel, &pulse, sizeof(rt_uint32_t)) != sizeof(rt_uint32_t))
	{
		rt_kprintf("write pwm channel %d: faild! \n", h_heat->PWM_channel);
		result = -RT_ERROR;
		goto _exit;
	}	
_exit:
	return result;
}

/*
 * 
 */
int head_system_init(void)
{
	heat_init(&HeatHandle_1);
	
	return 0;
}
INIT_APP_EXPORT(head_system_init);
/*
 * 开始加热
 */
rt_err_t start_heat(struct HeatSystem_t* h_heat)
{
	rt_err_t result = RT_EOK;
	struct rt_pwm_configuration configuration;
		
	result = rt_device_open(h_heat->dev, RT_DEVICE_FLAG_RDWR);
	if(result != RT_EOK)
	{
		rt_kprintf("open pwm faild! \n");
		result = -RT_EIO;
		goto _exit;
	}
	configuration.channel = h_heat->PWM_channel;
	configuration.period = TIME_FREQUENCY;    // 10e9 / period = frequency 
	configuration.pulse = 0;
	
	if( rt_device_control(h_heat->dev, PWM_CMD_SET, &configuration) != RT_EOK )
	{
		result = -RT_ERROR;
		goto _exit;
	}
	rt_device_control(h_heat->dev, PWM_CMD_ENABLE, &configuration);
_exit:
	return result;
}
/*
 * 停止加热
 */
rt_err_t stop_heat(struct HeatSystem_t* h_heat)
{
	rt_err_t result = RT_EOK;
	struct rt_pwm_configuration configuration;
	
	configuration.channel = h_heat->PWM_channel;
	result = rt_device_close(h_heat->dev);
	if(result != RT_EOK)
	{
		//rt_kprintf("close %s faild! \n", argv);
		result = -RT_EIO;
		goto _exit;
	}
	rt_device_control(h_heat->dev, PWM_CMD_DISABLE, &configuration);
_exit:
	return result;
}


