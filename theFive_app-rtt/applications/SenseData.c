#include "board.h"
#include "SenseData.h"
#include "drv_ad1110.h"
#include "Uart_Screen.h"
#include "dc_motor.h"
#include "usb_hid.h"

struct light_handle_t h_light_1;
struct light_handle_t h_light_2;
struct light_handle_t h_light_3;
struct light_handle_t h_light_4;
struct switch_config_t switch_config;
struct status_config_t status_config;
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
 *	获取杯位状态
 *	_ch ：通道
 *
 *	return : 1 --> 有杯
 *			 0 --> 无杯
 */
rt_uint8_t get_cup_status(rt_uint8_t _ch)
{
	rt_uint8_t* cup_status[4] = {&status_config.cup1_status, &status_config.cup2_status,
									&status_config.cup3_status, &status_config.cup4_status};

	return *cup_status[_ch];
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
/**
 *	使能光路检测
 *
 * _config : 1 --> en
 * 			 0 --> dis
 */
void en_sense_light(rt_uint8_t _ch, rt_uint8_t config, rt_uint8_t led_sw)
{
	rt_uint8_t* en_light[4] = {&switch_config.en_Light_1, &switch_config.en_Light_2,
								&switch_config.en_Light_3, &switch_config.en_Light_4};
	rt_uint8_t led_gpio[4] = {LED1_gpio, LED3_gpio, LED5_gpio, LED7_gpio};
	RT_ASSERT(_ch < 4);
	RT_ASSERT(config < 2);
	rt_pin_write(led_gpio[_ch], led_sw);
	*en_light[_ch] = config;
}
/*
 * 获取光度值  (耗时1.2ms)
 *
 * _config : 前4 bit为通道信息，后4 bit为使能管理
 * 			 					bit 0: 340
 * 			 					bit 1: 740
 *
 * out_data ： 输出数据（一共可以输出2个32位的数据）
 *
 * return ：0  -->输出数据 有效
 * 			1  -->输出数据 无效
 * */
rt_uint8_t Get_AdsData(rt_uint8_t _config, rt_uint32_t* out_data)
{ 
	static rt_uint8_t count[4] = {0};
	static rt_uint32_t ADLightDataSum[4][2] = {0};
	rt_uint8_t _channle = _config >> 4;
	rt_uint8_t _data[2][3] = {0};
	rt_uint16_t _val_1, _val_2;
	//static rt_uint32_t test_value = 0;

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
			//test_value++;
			//out_data[0] = test_value;
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
 *	显示光度值
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
	static rt_uint32_t ave_buf[4][2] = {{0,0}};
	rt_uint8_t a1_a2_flag[4] = {0};
	rt_uint32_t read_buf[2];

	for(rt_uint8_t i = 0; i<4; i++)
	{
		if(*en_read_light[i])
		{
			if(Get_AdsData(i<<4 | 1, read_buf))
				continue;
			dis_light_result(i, read_buf);

			if(ppLight[i]->pA_count != RT_NULL)
			{
				ave_buf[i][0] += read_buf[0];
				ave_buf[i][1] += read_buf[1];
				a1_a2_flag[i] = 1;
				//rt_kprintf("* %d\n", *ppLight[i]->pA_count);
				*ppLight[i]->pA_count -= 1;
				if(*ppLight[i]->pA_count == 0)
				{
					a1_a2_flag[i] = 0;
					ppLight[i]->pA_count[0] = -1;
					ppLight[i]->pA_count = RT_NULL;
					*ppLight[i]->pA_ave_1 = ave_buf[i][0] / 10;
					*ppLight[i]->pA_ave_2 = ave_buf[i][1] / 10;
					ppLight[i]->pA_ave_1 = RT_NULL;
					ppLight[i]->pA_ave_2 = RT_NULL;
					ave_buf[i][0] = 0;
					ave_buf[i][1] = 0;
				}
			}

			send_data_windos(i, read_buf[0], a1_a2_flag[i]);
			rt_kprintf("ch%d: %d  ", i, read_buf[0]);
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
		cup_cheak();
		door_sense_scan();
		//HAL_GPIO_TogglePin(GPIOF, GPIO_PIN_5);
		rt_thread_delay(100); 
	}
}
