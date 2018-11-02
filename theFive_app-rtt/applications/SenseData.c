#include "SenseData.h"
#include "drv_ad1110.h"
#include "bsp_ds18b20.h"
#include "Uart_Screen.h"
#include "Heat_PID.h"
#include "dc_motor.h"

struct light_handle_t h_light_1;
struct light_handle_t h_light_2;
struct light_handle_t h_light_3;
struct light_handle_t h_light_4;
struct switch_config_t switch_config;
struct status_config_t status_config;
/**
 *��λ���IO��ʼ��
 */
static void cup_gpio_init(void)
{
	rt_pin_mode(CupSense1_gpio, PIN_MODE_INPUT_PULLUP);
	rt_pin_mode(CupSense2_gpio, PIN_MODE_INPUT_PULLUP);
	rt_pin_mode(CupSense3_gpio, PIN_MODE_INPUT_PULLUP);
	rt_pin_mode(CupSense4_gpio, PIN_MODE_INPUT_PULLUP);
}
/**
 *��λ���
 */
static void cup_cheak(void)
{
	rt_uint8_t CupSense_gpio[4] = {CupSense1_gpio, CupSense2_gpio, CupSense3_gpio, CupSense4_gpio};
	rt_uint8_t* cup_en[4] = {&switch_config.en_cup_1, &switch_config.en_cup_2,
								&switch_config.en_cup_3, &switch_config.en_cup_4};
	rt_uint8_t* cup_status[4] = {&status_config.cup1_status, &status_config.cup2_status,
									&status_config.cup3_status, &status_config.cup4_status};

	for(rt_uint8_t i = 0; i<4; i++)
	{
		if(0 == *cup_en[i])
			continue;
		if(0 == rt_pin_read(CupSense_gpio[i]))
		{
			*cup_status[i] = 1;
			//rt_kprintf("cup %d : 1\n", i);
		}
		else
		{
			*cup_status[i] = 0;
			//rt_kprintf("cup %d : 0\n", i);
		}
	}
}
/**
 *	��ȡ��λ״̬
 *	_ch ��ͨ��
 *
 *	return : 1 --> �б�
 *			 0 --> �ޱ�
 */
rt_uint8_t get_cup_status(rt_uint8_t _ch)
{
	rt_uint8_t* cup_status[4] = {&status_config.cup1_status, &status_config.cup2_status,
									&status_config.cup3_status, &status_config.cup4_status};

	return *cup_status[_ch];
}
/*
 *��ͨ��������ʼ��
 * */
static int light_para_init(void)
{
	h_light_1.channel = 1;
	h_light_1.dis_addr_1 = LIGHT_1;
	h_light_1.dis_addr_2 = LIGHT_2;
	h_light_1.dis_icon_1 = LED1_ICO;
	h_light_1.dis_icon_2 = LED2_ICO;

	h_light_2.channel = 2;
	h_light_2.dis_addr_1 = LIGHT_3;
	h_light_2.dis_addr_2 = LIGHT_4;
	h_light_2.dis_icon_1 = LED3_ICO;
	h_light_2.dis_icon_2 = LED4_ICO;

	h_light_3.channel = 3;
	h_light_3.dis_addr_1 = LIGHT_5;
	h_light_3.dis_addr_2 = LIGHT_6;
	h_light_3.dis_icon_1 = LED5_ICO;
	h_light_3.dis_icon_2 = LED6_ICO;

	h_light_4.channel = 4;
	h_light_4.dis_addr_1 = LIGHT_7;
	h_light_4.dis_addr_2 = LIGHT_8;
	h_light_4.dis_icon_1 = LED7_ICO;
	h_light_4.dis_icon_2 = LED8_ICO;

    return RT_EOK;
}
/*
 * ��ȡ���ֵ  (��ʱ1.2ms)
 *
 * _config : ǰ4 bitΪͨ����Ϣ����4 bitΪʹ�ܹ���
 * 			 					bit 0: 340
 * 			 					bit 1:	740
 *
 * out_data �� ������ݣ�һ���������2��32λ�����ݣ�
 *
 * return ��0  -->������� ��Ч
 * 			1  -->������� ��Ч
 * */
rt_uint8_t Get_AdsData(rt_uint8_t _config, rt_uint32_t* out_data)
{ 
	static rt_uint8_t count[4] = {0};
	static rt_uint32_t ADLightDataSum[4][2] = {0};
	rt_uint8_t _channle = _config >> 4;
	rt_uint8_t _data[2][3] = {0};
	rt_uint16_t _val_1, _val_2;

	if(_config & 0x01)
	{
		ad1110_read_reg(_channle+1, ED0_ADDR, _data[0]);
	}
	if(_config & 0x02)
	{
		ad1110_read_reg(_channle+1, ED2_ADDR, _data[1]);
	}
	
	if(0 == (_data[0][2] & 0x80) || 0 == (_data[1][2] & 0x80))
	{
		_val_1 = _data[0][0];
		_val_2 = _data[1][0];
		_val_1 = (_val_1<<8) | _data[0][1];
		_val_2 = (_val_2<<8) | _data[1][1];
		//rt_kprintf("~~value : %d , %d\r\n",_val_1, _val_2);
		ADLightDataSum[_channle][0] += _val_1;
		ADLightDataSum[_channle][1] += _val_2;
		if(++count[_channle] >= 10)
		{
			count[_channle] = 0;
			out_data[0] = ADLightDataSum[_channle][0] / 10;
			out_data[1] = ADLightDataSum[_channle][1] / 10;
			ADLightDataSum[_channle][0] = 0;
			ADLightDataSum[_channle][1] = 0;
			return RT_EOK;
		}
	}
	out_data[0] = out_data[1] = 0;

	return RT_ERROR;
}
/**
 *	��ʾ���ֵ
 */
static void dis_light_result(rt_uint8_t _ch, rt_uint32_t* pdata)
{
	rt_uint16_t light_LCD[4][2] = {{LIGHT_1, LIGHT_2},{LIGHT_3, LIGHT_4},
									{LIGHT_5, LIGHT_6},{LIGHT_7, LIGHT_8}};

	ScreenSendData_2bytes(light_LCD[_ch][0], (int)pdata[0]);
	ScreenSendData_2bytes(light_LCD[_ch][1], (int)pdata[1]);
}
static void cycle_read_light(void)
{
	rt_uint8_t *en_read_light[4] = {&switch_config.en_Light_1, &switch_config.en_Light_2,  \
									&switch_config.en_Light_3, &switch_config.en_Light_4};
	struct light_handle_t *ppLight[4] = {&h_light_1, &h_light_2, &h_light_3, &h_light_4};
	rt_uint32_t read_buf[2];

	for(rt_uint8_t i = 0; i<4; i++)
	{
		if(*en_read_light[i])
		{
			if(Get_AdsData(i<<4 | 3, read_buf))
				continue;
			dis_light_result(i, read_buf);
    		//rt_kprintf("ch:%d. %d--%d\n", i, read_buf[0], read_buf[1]);
			if(ppLight[i]->a1_status > 0)
			{
				ppLight[i]->ave_a1_1 += read_buf[0];
				ppLight[i]->ave_a1_2 += read_buf[1];
				//rt_kprintf("a11\n");
				ppLight[i]->a1_status--;
			}
			else if(ppLight[i]->a1_status == 0)
			{
				ppLight[i]->a1_status = -1;
				ppLight[i]->ave_a1_1 /= 10;
				ppLight[i]->ave_a1_2 /= 10;
			}
			if(ppLight[i]->a2_status > 0)
			{
 				ppLight[i]->ave_a2_1 += read_buf[0];
				ppLight[i]->ave_a2_2 += read_buf[1];
				//rt_kprintf("a22\n");
				ppLight[i]->a2_status--;
			}
			else if(ppLight[i]->a2_status == 0)
			{
				ppLight[i]->a2_status = -1;
				ppLight[i]->ave_a2_1 /= 10;
				ppLight[i]->ave_a2_2 /= 10;
			}
		}
	}
}
/**
 * @brief  ��ȡʵʱ�¶�
 *
 * _ch : ͨ��
 * @return �����¶�ֵ��100��
 */
rt_uint16_t GetTempAndDisplay(rt_uint8_t _ch)
{
	static rt_uint8_t _count[4] = {0};
	static rt_uint16_t _val[4] = {0};
	OneWire_t *pOneWire[4] = {&OneWire1, &OneWire2, &OneWire3, &OneWire4};
	rt_uint16_t dis_addr[4] = {TEMP_1, TEMP_2, TEMP_3, TEMP_4};
	rt_uint8_t* temp_status[4] = {&status_config.temp1_status, &status_config.temp2_status,
									&status_config.temp3_status, &status_config.temp4_status};
	
	if(++_count[_ch] > 4)
	{
		_count[_ch] = 0;
		_val[_ch] = ds18b20_read_reg(pOneWire[_ch]);
		_val[_ch] = (_val[_ch] * 10) >> 4;
		//rt_kprintf("ch:%d, %d c\n", _ch+1, _val[_ch]);
		ScreenSendData_2bytes(dis_addr[_ch], _val[_ch]);
		if(_val[_ch] > 3690 && _val[_ch] < 3710)   // ����¶�״̬
			*temp_status[_ch] = 1;
		else
			*temp_status[_ch] = 0;
	}
	return _val[_ch];
}
/**
 *	��ȡ�¶�״̬
 *	_ch ��ͨ��
 *
 *	return : 1 --> �¶ȵ���37
 *			 0 --> �¶� δ��
 */
rt_uint8_t get_temp_status(rt_uint8_t _ch)
{
	rt_uint8_t* temp_status[4] = {&status_config.temp1_status, &status_config.temp2_status,
									&status_config.temp3_status, &status_config.temp4_status};

	return *temp_status[_ch];
}
/**
 *	����ϵͳ
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
		if(*en_heat[i])
		{
			cycle_heat_count++;
			if(cycle_heat_count == pHeatHandle[i]->CycleTime)
			{
				cycle_heat_count = 0;
				pHeatHandle[i]->duty = (rt_uint16_t)pid_calculate(pHeatHandle[i]);
				set_heat_duty(pHeatHandle[i], pHeatHandle[i]->duty);
			}
		}
	}
}

void Function_SenseData(void* parameter)
{
	light_para_init();
	cup_gpio_init();
	door_start(0, 0);
	door_start(1, 0);

	while(1)
	{
		cycle_read_light();
		cycle_temp_heat();
		cup_cheak();
		door_sense_scan();
		HAL_GPIO_TogglePin(GPIOF, GPIO_PIN_5);
		rt_thread_delay(100); 
	}
}
