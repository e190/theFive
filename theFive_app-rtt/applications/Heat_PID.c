#include <stdio.h>
#include <string.h>
#include "Heat_PID.h"
#include "bsp_ds18b20.h"
#include "Uart_Screen.h"
#include "SenseData.h"

struct HeatSystem_t HeatHandle_1, HeatHandle_2, HeatHandle_3, HeatHandle_4;
rt_device_t heat_dev;
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
	
	if(h_heat->iCurVal < 200)	//低于20度，不正常
		return 0;
	_pid->err = h_heat->iSetVal - h_heat->iCurVal;
	if (_pid->err > 10)
	{
		index = 0;
		_pid->integral = 0;
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
		_temp = 1;

	return _temp;
}

void set_heat_pos()
{

}
/*
 * 初始化加热系统
 * PID : P-->4  I-->0.02 D-->50
 *
 */
void heat_para_init(struct HeatSystem_t* h_heat, rt_uint8_t _ch)
{
	h_heat->PWM_channel = _ch;
	h_heat->PID.uKP_Coe = 20;
	h_heat->PID.uKI_Coe = 0.1;
	h_heat->PID.uKD_Coe = 80;
	h_heat->PID.liEkVal[0] = 0;
	h_heat->PID.liEkVal[1] = 0;
	h_heat->PID.liEkVal[2] = 0;
	h_heat->iSetVal = 370;
	h_heat->iCurVal = 0;
	h_heat->CycleTime = 2;
}
/*
 * 设置PWM占空比
 * percent : 占空比 * 1000
 */
int set_heat_duty(struct HeatSystem_t* h_heat, rt_uint16_t percent)
{
	rt_err_t result = RT_EOK;
	rt_uint32_t pulse = 0;
		
	pulse = (TIME_FREQUENCY / 100) * percent ;
	if( rt_device_write(heat_dev, h_heat->PWM_channel, &pulse, sizeof(rt_uint32_t)) != sizeof(rt_uint32_t))
	{
		rt_kprintf("write pwm channel %d: faild! \n", h_heat->PWM_channel);
		result = -RT_ERROR;
		goto _exit;
	}	
_exit:
	return result;
}
/**
 *	获取温度状态
 *	_ch ：通道
 *
 *	return : 1 --> 温度到达37
 *			 0 --> 温度 未到
 */
rt_uint8_t get_temp_status(rt_uint8_t _ch)
{
	rt_uint8_t* temp_status[4] = {&status_config.temp1_status, &status_config.temp2_status,
									&status_config.temp3_status, &status_config.temp4_status};

	return *temp_status[_ch];
}
/**
 * @brief  获取实时温度(目前1s刷新)
 *
 * _ch : 通道
 * @return 返回温度值的100倍
 */
rt_uint16_t GetTempAndDisplay(rt_uint8_t _ch)
{
	static rt_uint16_t _val[4] = {0};
	OneWire_t *pOneWire[4] = {&OneWire1, &OneWire2, &OneWire3, &OneWire4};
	rt_uint16_t dis_addr[4] = {TEMP_1, TEMP_2, TEMP_3, TEMP_4};
	rt_uint8_t* temp_status[4] = {&status_config.temp1_status, &status_config.temp2_status,
									&status_config.temp3_status, &status_config.temp4_status};
	static rt_uint8_t print_count = 0;

	_val[_ch] = ds18b20_read_reg(pOneWire[_ch]);
	_val[_ch] = (_val[_ch] * 10) >> 4;
	if(++print_count > 10)
	{
		print_count = 0;
		//rt_kprintf("%d\n", _val[_ch]);
	}
	//rt_kprintf("ch%d: %d\n", _ch+1, _val[_ch]);
	ScreenSendData_2bytes(dis_addr[_ch], _val[_ch]);
	if(_val[_ch] > 3690 && _val[_ch] < 3710)   // 标记温度状态
		*temp_status[_ch] = 1;
	else
		*temp_status[_ch] = 0;

	return _val[_ch];
}
/**
 *	加热系统
 */
void cycle_temp_heat(void)
{
	rt_uint8_t *en_read_temp[4] = {&switch_config.en_Temp_1, &switch_config.en_Temp_2,  \
									&switch_config.en_Temp_3, &switch_config.en_Temp_4};
	rt_uint8_t *en_heat[4] = {&switch_config.en_Heat_1, &switch_config.en_Heat_2,  \
									&switch_config.en_Heat_3, &switch_config.en_Heat_4};
	struct HeatSystem_t *pHeatHandle[4] = {&HeatHandle_1, &HeatHandle_2, &HeatHandle_3, &HeatHandle_4};
	static rt_uint8_t cycle_heat_count[4] = {0};

	for(rt_uint8_t i = 0; i<4; i++)
	{
		if(*en_read_temp[i] || *en_heat[i])
		{
			if(++cycle_heat_count[i] == pHeatHandle[i]->CycleTime)
			{
				cycle_heat_count[i] = 0;
				pHeatHandle[i]->iCurVal = (float)GetTempAndDisplay(i);
				if(*en_heat[i])
				{
					pHeatHandle[i]->duty = (rt_uint16_t)pid_calculate(pHeatHandle[i]);
					set_heat_duty(pHeatHandle[i], pHeatHandle[i]->duty);
				}
			}
		}
	}
}
static int heat_pwm_init(void)
{
	rt_err_t result = RT_EOK;
	struct rt_pwm_configuration configuration;

	heat_dev = rt_device_find(heat_name);
	if(!heat_dev)
	{
		rt_kprintf("%s not found!\n", heat_name);
		result = -RT_ERROR;
		goto _exit;
	}
	result = rt_device_open(heat_dev, RT_DEVICE_FLAG_RDWR);
	if(result != RT_EOK)
	{
		rt_kprintf("open %s faild! \n", heat_name);
		result = -RT_EIO;
		goto _exit;
	}

	configuration.period = TIME_FREQUENCY;    // 10e9 / period = frequency
	configuration.pulse = configuration.period / 2;
	for(rt_uint8_t i=0;i<4;i++)
	{
		configuration.channel = i;
		if( rt_device_control(heat_dev, PWM_CMD_SET, &configuration) != RT_EOK )
		{
			rt_kprintf("control PWM_CMD_SET channel %d: faild! \n", configuration.channel);
			result = -RT_ERROR;
			goto _exit;
		}
		//rt_device_control(heat_dev, PWM_CMD_ENABLE, &configuration);
	}
	configuration.channel = 0;
	//rt_device_control(heat_dev, PWM_CMD_ENABLE, &configuration);

	_exit:
		return result;
}
/*
 * 开始加热
 */
rt_err_t heat_start_stop(struct HeatSystem_t* h_heat, HeatSwitch _status)
{
	struct rt_pwm_configuration configuration;
		
	configuration.channel = h_heat->PWM_channel;
	if(_status == HEAT_START)
		return rt_device_control(heat_dev, PWM_CMD_ENABLE, &configuration);
	else
		return rt_device_control(heat_dev, PWM_CMD_DISABLE, &configuration);
}
/*
 *
 */
int head_system_init(void)
{
	heat_pwm_init();
	heat_para_init(&HeatHandle_1, 0);
	heat_para_init(&HeatHandle_2, 1);
	heat_para_init(&HeatHandle_3, 2);
	heat_para_init(&HeatHandle_4, 3);

	return 0;
}
INIT_APP_EXPORT(head_system_init);
