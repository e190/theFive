#include "SenseData.h"
#include "drv_ad1110.h"
#include "bsp_ds18b20.h"
#include "Uart_Screen.h"
#include "Heat_PID.h"

struct light_handle_t h_light_1;
struct light_handle_t h_light_2;
struct light_handle_t h_light_3;
struct light_handle_t h_light_4;
struct switch_config_t switch_config;
/**
 *杯位检测IO初始化
 */
static void cup_gpio_init(void)
{
	rt_pin_mode(CupSense1_gpio, PIN_MODE_INPUT_PULLUP);
	rt_pin_mode(CupSense2_gpio, PIN_MODE_INPUT_PULLUP);
	rt_pin_mode(CupSense3_gpio, PIN_MODE_INPUT_PULLUP);
	rt_pin_mode(CupSense4_gpio, PIN_MODE_INPUT_PULLUP);
}
/**
 *杯位检测
 */
static void cup_cheak(void)
{
	rt_uint8_t CupSense_gpio[4] = {CupSense1_gpio, CupSense2_gpio, CupSense3_gpio, CupSense4_gpio};
	rt_uint8_t* cup_en[4] = {&switch_config.en_cup_1, &switch_config.en_cup_2,
								&switch_config.en_cup_3, &switch_config.en_cup_4};

	for(rt_uint8_t i = 0; i<4; i++)
	{
		if(0 == *cup_en[i])
			continue;
		if(0 == rt_pin_read(CupSense_gpio[i]))
			rt_kprintf("1 coming\n");
		else
			rt_kprintf("1 coming\n");
	}

}
/*
 *光通道参数初始化
 * */
static int light_para_init(void)
{
	h_light_1.channel = 1;
	h_light_1.dis_addr_1 = LIGHT_1;
	h_light_1.dis_addr_2 = LIGHT_2;
	h_light_1.dis_icon_1 = LED1_ICO;
	h_light_1.dis_icon_2 = LED2_ICO;

    return RT_EOK;
}

/*
 * 获取光度值
 *
 * _config : bit 0: 340
 * 			 bit 1:	740
 * */
static void Get_AdsData(struct light_handle_t *p_light, rt_uint8_t _config)
{ 
	static rt_uint8_t count = 0, s_count = 0;
	static rt_uint32_t ADLightDataSum1, ADLightDataSum2;
	rt_uint8_t _date[2][3] = {0};
	rt_uint16_t _val_1, _val_2;

	if(_config & 0x01)
	{
		ad1110_read_reg(p_light->channel, ED0_ADDR, _date[0]);
		ScreenDisICON(p_light->dis_icon_1, 1);
	}
	else
		ScreenDisICON(p_light->dis_icon_1, 0);
	rt_thread_delay(10);
	if(_config & 0x02)
	{
		ad1110_read_reg(p_light->channel, ED2_ADDR, _date[1]);
		ScreenDisICON(p_light->dis_icon_2, 1);
	}
	else
		ScreenDisICON(p_light->dis_icon_2, 0);
	
	if(0 == (_date[0][2] & 0x80) && 0 == (_date[1][2] & 0x80))
	{
		_val_1 = _date[0][0];
		_val_2 = _date[1][0];
		_val_1 = (_val_1<<8) | _date[0][1];
		_val_2 = (_val_2<<8) | _date[1][1];
		//rt_kprintf("~~value : %d , %d\r\n",_val_1, _val_2);

		ADLightDataSum1 += _val_1;
		ADLightDataSum2 += _val_2;
		count++;
		if(count >= 10)
		{
			count = 0;
			ADLightDataSum1 = ADLightDataSum1 / 10;
			ADLightDataSum2 = ADLightDataSum2 / 10;
			//rt_kprintf("%d\n",ADLightDataSum2);
			if(p_light->a1_status)
			{
				p_light->ave_a1_1 += ADLightDataSum1;
				p_light->ave_a1_2 += ADLightDataSum2;
				//rt_kprintf("a11\n");
				if(++s_count >= 10)
				{
					p_light->a1_status = 0;
					p_light->ave_a1_1 /= 10;
					p_light->ave_a1_2 /= 10;
					s_count = 0;
				}
			}
			if(h_light_1.a2_status)
			{
				h_light_1.ave_a2_1 += ADLightDataSum1;
				h_light_1.ave_a2_2 += ADLightDataSum2;
				//rt_kprintf("a22\n");
				if(++s_count >= 10)
				{
					p_light->a2_status = 0;
					p_light->ave_a2_1 /= 10;
					p_light->ave_a2_2 /= 10;
					s_count = 0;
				}
			}
			if(_config & 0x01)
				ScreenSendData_2bytes(p_light->dis_addr_1, (int)ADLightDataSum1);
			rt_thread_delay(10);
			if(_config & 0x02)
				ScreenSendData_2bytes(p_light->dis_addr_2, (int)ADLightDataSum2);
			ADLightDataSum1 = 0;
			ADLightDataSum2 = 0;
		}
	}	
}
static void cycle_read_light(void)
{
	rt_uint8_t *en_read_light[4] = {&switch_config.en_Light_1, &switch_config.en_Light_1,  \
									&switch_config.en_Light_1, &switch_config.en_Light_1};
	struct light_handle_t *ppLight[4] = {&h_light_1, &h_light_2, &h_light_3, &h_light_4};

	for(rt_uint8_t i = 0; i<4; i++)
	{
		if(0 == *en_read_light[i])
			continue;
		Get_AdsData(ppLight[i], 3);
	}
}
/**
 * @brief  获取实时温度
 *
 * _ch : 通道
 * @return 返回温度值的100倍
 */
rt_uint16_t GetTempAndDisplay(rt_uint8_t _ch)
{
	static rt_uint8_t i = 0;
	static rt_uint16_t _val = 0;
	OneWire_t *pOneWire[4] = {&OneWire1, &OneWire2, &OneWire3, &OneWire4};
	rt_uint16_t dis_addr[4] = {TEMP_1, TEMP_2, TEMP_3, TEMP_4};
	
	i++;
	if(i > 5)
	{
		i = 0;
		//_val = ds18b20_read_ID(&OneWire1);
		_val = ds18b20_read_reg(pOneWire[_ch]);
		_val = (_val * 10) >> 4;
		//rt_kprintf( "%d\n",_val);
		ScreenSendData_2bytes(dis_addr[_ch], _val);
	}
	return _val;
}
/**
 *	加热系统
 */
static void cycle_temp_heat(void)
{
	rt_uint8_t *en_read_temp[4] = {&switch_config.en_Temp_1, &switch_config.en_Temp_2,  \
									&switch_config.en_Temp_3, &switch_config.en_Temp_4};
	rt_uint8_t *en_heat[4] = {&switch_config.en_Heat_1, &switch_config.en_Heat_2,  \
									&switch_config.en_Heat_3, &switch_config.en_Heat_4};
	struct HeatSystem_t *pHeatHandle[4] = {&HeatHandle_1, &HeatHandle_2, &HeatHandle_3, &HeatHandle_4};
	static rt_uint8_t cycle_heat_count = 0;

	for(rt_uint8_t i = 0; i<4; i++)
	{
		if(*en_read_temp[i] || *en_heat[i])
			pHeatHandle[i]->iCurVal = (float)GetTempAndDisplay(i);
		if(switch_config.en_Heat_1)
		{
			cycle_heat_count++;
			if(cycle_heat_count == pHeatHandle[i]->CycleTime)
			{
				cycle_heat_count = 0;
				pHeatHandle[i]->duty = (rt_uint16_t)pid_calculate(pHeatHandle[i]);
				set_duty(pHeatHandle[i], pHeatHandle[i]->duty);
			}
		}
	}
}

void Function_SenseData(void* parameter)
{
	light_para_init();
	cup_gpio_init();

	while(1)
	{
		cycle_read_light();
		cycle_temp_heat();
		cup_cheak();
		rt_thread_delay(100); 
	}
}
