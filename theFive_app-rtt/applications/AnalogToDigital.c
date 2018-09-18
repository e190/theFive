#include "AnalogToDigital.h"
#include "drv_ad1110.h"
#include "bsp_ds18b20.h"
#include "Uart_Screen.h"
#include "Heat_PID.h"

light_handle_t h_light_1;
/*
 * 获取光度值
 * */
void Get_AdsData(void)
{ 
	static rt_uint8_t i = 0, n;
	rt_uint8_t _date[3];
	rt_uint16_t _val;
	char str[10] = {0};
	static rt_uint32_t ADLightDataSum;

	ad1110_read_reg(ED0_ADDR, 0x8c, _date);
	
	if(0 == (_date[2] & 0x80))
	{
		_val = _date[0];
		_val = (_val<<8) | _date[1];  
		//rt_kprintf("~~value : %d\r\n",_val);
		ADLightDataSum += _val;
		i++;
		if(i >= 10)
		{
			i = 0;
			ADLightDataSum = ADLightDataSum / 10;
			rt_kprintf("%d\r\n",ADLightDataSum);
			if(h_light_1.a1_status)
			{
				h_light_1.ave_a1 += ADLightDataSum;
				//rt_kprintf("a11\n");
				if(++n >= 10)
				{
					h_light_1.a1_status = 0;
					h_light_1.ave_a1 /= 10;
					n = 0;
				}
			}
			if(h_light_1.a2_status)
			{
				h_light_1.ave_a2 += ADLightDataSum;
				//rt_kprintf("a22\n");
				if(++n >= 10)
				{
					h_light_1.a2_status = 0;
					h_light_1.ave_a2 /= 10;
					n = 0;
				}
			}
			rt_memset(str, 0, sizeof(str));
			sprintf(str, "%d", (int)ADLightDataSum);
			ScreenSendCommand(WRITE_82, LIGHT_1, (rt_uint8_t*)str,sizeof(str));
			ADLightDataSum = 0;
		}
	}	
}
/**
 * @brief  获取实时温度
 *  
 * @param 
 *
 * @return 返回温度值的100倍
 */
rt_uint16_t GetTempAndDisplay(void)
{
	static rt_uint8_t i = 0;
	static rt_uint16_t _val = 0;
	
	i++;
	if(i > 5)
	{
		i = 0;
		//_val = ds18b20_read_ID(&OneWire1);
		_val = ds18b20_read_reg(&OneWire1);
		_val = (_val * 10) >> 4;
		//rt_kprintf( "%d\n",_val);
		ScreenSendData(TEMP_1, (rt_uint8_t*)&_val, 2);	
	}
	return _val;
}


void Function_AnalogToDigital(void* parameter)
{
	rt_uint8_t i = 0;
	
	while(1)
	{
		if(switch_config.en_Light_1)
			Get_AdsData();	
		if(switch_config.en_Temp_1 || switch_config.en_Heat_1)
			HeatHandle1.iCurVal = (float)GetTempAndDisplay();
		if(switch_config.en_Heat_1)
		{
			i++;
			if(i == HeatHandle1.CycleTime)
			{
				i = 0;
				HeatHandle1.duty = (rt_uint16_t)pid_calculate(&HeatHandle1);
				set_duty(&HeatHandle1, HeatHandle1.duty);
			}
		}
		rt_thread_delay(100); 
	}
}
