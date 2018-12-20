#include <rtthread.h>
#include <stdio.h>
#include <string.h>
#include "Heat_PID.h"
#include "bsp_ds18b20.h"
#include "Uart_Screen.h"
#include "SenseData.h"
#include "DataBase.h"

#define LOG_TAG  "heat"
#define DBG_ENABLE
#define DBG_SECTION_NAME               "pid"
#define DBG_LEVEL                      DBG_INFO
#define DBG_COLOR
#include <rtdbg.h>

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
	struct PID_Value* _pid = &h_heat->PID;
	
	if(h_heat->iCurVal < 100)	//低于10度，不正常
		return 0;
	_pid->err = _pid->iSetVal - h_heat->iCurVal;
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
/**
 *	设置加热系统参数
 *	_ch ： 通道号  （0 - 3）
 */
void set_heat_para(rt_uint8_t _ch, const struct PID_Value* handle)
{
	struct PID_Value* pPid_pos[4] = {&HeatHandle_1.PID, &HeatHandle_2.PID,
										  &HeatHandle_3.PID, &HeatHandle_4.PID};
	RT_ASSERT(_ch < 4);

	/* 判断参数是否有修改 */
	if(rt_memcmp(pPid_pos[_ch], handle, sizeof(struct PID_Value) - 12))
	{
		if(handle->iSetVal != 0)
		{
			pPid_pos[_ch]->iSetVal = handle->iSetVal;
		}

		if(handle->CycleTime != 0)
		{
			pPid_pos[_ch]->CycleTime = handle->CycleTime;
		}

		if(handle->uKP_Coe != 0)
		{
			pPid_pos[_ch]->uKP_Coe = handle->uKP_Coe;
		}

		if(handle->uKI_Coe != 0)
		{
			pPid_pos[_ch]->uKI_Coe = handle->uKI_Coe;
		}

		if(handle->uKD_Coe != 0)
		{
			pPid_pos[_ch]->uKD_Coe = handle->uKD_Coe;
		}

		set_system_para_cache(HEAT_PARA_POS + sizeof(struct PID_Value) * (_ch - 1),
				pPid_pos[_ch], sizeof(struct PID_Value));
		fresh_system_para();
	}
}
/**
 *	读取加热系统参数
 *	_ch ： 通道号  （0 - 3）
 */
int read_heat_para(rt_uint8_t _ch, struct PID_Value* handle)
{
	struct HeatSystem_t* pHeatTemp[4] = {&HeatHandle_1, &HeatHandle_2,
										  &HeatHandle_3, &HeatHandle_4};
	RT_ASSERT(_ch < 4);

	rt_memcpy(handle, &pHeatTemp[_ch]->PID, sizeof(struct PID_Value));

	return RT_EOK;
}
/*
 * 初始化加热系统
 * PID : P-->4  I-->0.02 D-->50
 *	_ch ： 通道号  （1 - 4）
 */
void heat_para_init(struct HeatSystem_t* h_heat, rt_uint8_t _ch)
{
	h_heat->PWM_channel = _ch;
	h_heat->PID.integral = 0;
	h_heat->PID.err_last = 0;
	h_heat->PID.err = 0;
	h_heat->iCurVal = 0;
	get_system_para_cache(HEAT_PARA_POS + sizeof(h_heat->PID) * (_ch - 1),
			&h_heat->PID, sizeof(h_heat->PID));
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
		LOG_E("write pwm channel %d: faild! \n", h_heat->PWM_channel);
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
rt_uint8_t get_temp_a_status(rt_uint8_t _ch)
{
	rt_uint8_t* temp_status[4] = {&status_config.temp1_status, &status_config.temp2_status,
									&status_config.temp3_status, &status_config.temp4_status};

	return *temp_status[_ch];
}
/**
 *	获取温度状态
 *
 *	return : 0 --> 温度到达37
 *			 1 --> 温度 未到
 */
rt_uint8_t get_temp_status(void)
{
	rt_uint8_t temp_status = 0;

	for(rt_uint8_t i=0; i<4; i++)
	{
		if(get_temp_a_status(i))
			temp_status++;
	}
	if(temp_status == 4)
		return RT_EOK;
	else
		return RT_ERROR;
}
void dis_temp_status(void)
{
	char string[30] = {0};
	rt_uint8_t new_status, old_status = 2;

	if(status_config.system_init)
		return;
	new_status = get_temp_status();
	if(new_status != old_status)
	{
		if(new_status == 0)
		{
			rt_sprintf(string, "当前温度合适！");
			ScreenSendCommand(WRITE_82, INIT_INFO, (rt_uint8_t*)string, rt_strlen(string));
			ScreenPage(1);
			status_config.system_init = 1;
			switch_config.temp_dis = 0;
		}
		else
		{
			rt_sprintf(string, "当前温度不足，正在加热模块！");
			ScreenSendCommand(WRITE_82, INIT_INFO, (rt_uint8_t*)string, rt_strlen(string));
		}
		old_status = new_status;
	}
}

/**
 * @brief  获取实时温度(目前1s刷新)
 *
 * _ch : 通道
 * @return 返回温度值的100倍
 */
rt_uint16_t GetTemp(rt_uint8_t _ch)
{
	static rt_uint16_t _val[4] = {0};
	OneWire_t *pOneWire[4] = {&OneWire1, &OneWire2, &OneWire3, &OneWire4};
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
	if(_val[_ch] > 368 && _val[_ch] < 372)   // 标记温度状态
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

	for(rt_uint8_t i = 0; i < 4; i++)
	{
		if(*en_read_temp[i] || *en_heat[i])
		{
			if(++cycle_heat_count[i] == pHeatHandle[i]->PID.CycleTime)
			{
				cycle_heat_count[i] = 0;
				pHeatHandle[i]->iCurVal = (float)GetTemp(i);
				if(*en_heat[i])
				{
					pHeatHandle[i]->duty = (rt_uint16_t)pid_calculate(pHeatHandle[i]);
					set_heat_duty(pHeatHandle[i], pHeatHandle[i]->duty);
				}
				if(switch_config.temp_dis)
				{
					ScreenSendData_2bytes(TEMP_1 + i * 4, pHeatHandle[i]->iCurVal);
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
		LOG_E("%s not found!\n", heat_name);
		result = -RT_ERROR;
		goto _exit;
	}
	result = rt_device_open(heat_dev, RT_DEVICE_FLAG_RDWR);
	if(result != RT_EOK)
	{
		LOG_E("open %s faild! \n", heat_name);
		result = -RT_EIO;
		goto _exit;
	}

	configuration.period = TIME_FREQUENCY;    // 10e9 / period = frequency
	configuration.pulse = configuration.period / 2;
	for(rt_uint8_t i = 0; i < 4; i++)
	{
		configuration.channel = i + 1;
		if( rt_device_control(heat_dev, PWM_CMD_SET, &configuration) != RT_EOK )
		{
			LOG_E("control PWM_CMD_SET channel %d: faild! \n", configuration.channel);
			result = -RT_ERROR;
			goto _exit;
		}
	}
	configuration.channel = 0;
	//rt_device_control(heat_dev, PWM_CMD_ENABLE, &configuration);

	_exit:
		return result;
}
/*
 * 开始加热
 * _status 1：开始加热
 * 		   0：停止加热
 */
rt_err_t heat_start_stop(struct HeatSystem_t* h_heat, HeatSwitch _status)
{
	struct rt_pwm_configuration configuration;
		
	configuration.channel = h_heat->PWM_channel;
	if(_status == HEAT_START)
	{
		if(h_heat->PID.iSetVal == 0)
		{
			LOG_E("PID para error");
			return RT_ERROR;
		}
		h_heat->PID.integral = 0;
		h_heat->PID.err_last = 0;
		h_heat->PID.err = 0;
		return rt_device_control(heat_dev, PWM_CMD_ENABLE, &configuration);
	}
	else
		return rt_device_control(heat_dev, PWM_CMD_DISABLE, &configuration);
}
/*
 * 所有通道开始加热
 * _config 1：开始加热
 * 		   0：停止加热
 */
rt_err_t all_heat_start_stop(HeatSwitch _config)
{
	struct HeatSystem_t* pHeatTemp[4] = {&HeatHandle_1, &HeatHandle_2,
										  &HeatHandle_3, &HeatHandle_4};
	rt_uint8_t* en_heat[4] = {&switch_config.en_Heat_1, &switch_config.en_Heat_2,
								&switch_config.en_Heat_3, &switch_config.en_Heat_4};

	for(rt_uint8_t i = 0; i < 4; i++)
	{
		if(heat_start_stop(pHeatTemp[i], _config) != RT_EOK)
		{
			LOG_E("control heat %d: faild! \n", i);
			return RT_ERROR;
		}
		*en_heat[i] = _config;
	}
	return RT_EOK;
}
/*
 *	初始化加热系统
 */
int head_system_init(void)
{
	heat_pwm_init();
	heat_para_init(&HeatHandle_1, 1);
	heat_para_init(&HeatHandle_2, 2);
	heat_para_init(&HeatHandle_3, 3);
	heat_para_init(&HeatHandle_4, 4);

	return 0;
}
