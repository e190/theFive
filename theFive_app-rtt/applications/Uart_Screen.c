#include "Uart_Screen.h"
#include "RunLED.h"
#include "ServerData.h"
#include "WorkTask.h"
#include "bsp_RMD8.h"
#include "SenseData.h"
#include "TMC5130.h"

UartBuff_t UartBuff;

/* 串口接收事件标志 */
#define UART_RX_EVENT (1 << 0)

/* 事件控制块 */
static struct rt_event event;
/* 串口设备句柄 */
static rt_device_t uart_device = RT_NULL;
    
/* 回调函数 */
static rt_err_t uart_intput(rt_device_t dev, rt_size_t size)
{
    /* 发送事件 */
    rt_event_send(&event, UART_RX_EVENT);
    
    return RT_EOK;
}
 
rt_uint8_t uart_getchar(void)
{
    rt_uint32_t e;
    rt_uint8_t ch;

    /* 读取1字节数据 */
    while (rt_device_read(uart_device, 0, &ch, 1) != 1)
    {
         /* 接收事件 */
        rt_event_recv(&event, UART_RX_EVENT,RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR,RT_WAITING_FOREVER, &e);
    }

    return ch;
}
void uart_putchar(const rt_uint8_t c)
{
    rt_size_t len = 0;
    rt_uint32_t timeout = 0;
    do
    {
        len = rt_device_write(uart_device, 0, &c, 1);
        timeout++;
    }
    while (len != 1 && timeout < 500);
}
/**
 * @brief  
 *  
 * @param 
 *
 * @return
 */
static rt_uint8_t choose_channel(rt_uint32_t _addr, rt_uint8_t _channel)
{
	char str[2] = {0};
	
	switch (_channel)
	{
	case 1:		
		rt_sprintf(str, "一");
		ScreenSendCommand(WRITE_82, _addr, (rt_uint8_t*)str,rt_strlen(str));
		break;
	case 2:
		rt_sprintf(str, "二");
		ScreenSendCommand(WRITE_82, _addr, (rt_uint8_t*)str,rt_strlen(str));
		break;
	case 3:
		rt_sprintf(str, "三");
		ScreenSendCommand(WRITE_82, _addr, (rt_uint8_t*)str,rt_strlen(str));
		break;
	case 4:
		rt_sprintf(str, "四");
		ScreenSendCommand(WRITE_82, _addr, (rt_uint8_t*)str,rt_strlen(str));
		break;
	default:
		break;
	}
	
	return _channel;
}
static int sample_start(rt_uint16_t _addr)
{		
	if(_addr < SAMPLE_CHANNEL_1 || _addr > SAMPLE_CHANNEL_4)
		return RT_ERROR;
	rt_uint8_t num =  _addr - SAMPLE_CHANNEL_1;
	
	if(switch_config.sample_start & (0x01<<num))
	{
		ScreenDisICON(SAMPLE1_ICO + num, 0);
		switch_config.sample_start &= ~(0x01<<num);
	}
	else	
	{
		ScreenDisICON(SAMPLE1_ICO + num, 1);
		switch_config.sample_start |= 0x01 << num;	
	}
	return RT_EOK;
}
static int set_work_heat(rt_uint16_t _addr, rt_uint16_t value)
{
	rt_uint16_t* pheat_time[4] = {&sample_param_1.heat_time, &sample_param_2.heat_time,
									&sample_param_3.heat_time, &sample_param_4.heat_time};
	if(_addr < SAMPLE1_HEAT_TIME || _addr > SAMPLE4_HEAT_TIME)
		return RT_ERROR;
	*pheat_time[(_addr-SAMPLE1_HEAT_TIME)/2] = value;
	
	return RT_EOK;
}
static int set_work_read(rt_uint16_t _addr, rt_uint16_t value)
{
	rt_uint16_t* pread_time[4] = {&sample_param_1.read_time, &sample_param_2.read_time,
									&sample_param_3.read_time, &sample_param_4.read_time};
	if(_addr < SAMPLE1_READ_TIME || _addr > SAMPLE4_READ_TIME)
		return RT_ERROR;
	*pread_time[(_addr-SAMPLE1_READ_TIME)/2] = value;
	
	return RT_EOK;
}
static int set_work_a1(rt_uint16_t _addr, rt_uint16_t value)
{
	rt_uint16_t* pa1_time[4] = {&sample_param_1.a1_time, &sample_param_2.a1_time,
									&sample_param_3.a1_time, &sample_param_4.a1_time};
	if(_addr < SAMPLE1_A1_TIME || _addr > SAMPLE4_A1_TIME)
		return RT_ERROR;
	*pa1_time[(_addr-SAMPLE1_A1_TIME)/2] = value;
	
	return RT_EOK;
}
static int set_work_a2(rt_uint16_t _addr, rt_uint16_t value)
{
	rt_uint16_t* pa2_time[4] = {&sample_param_1.a2_time, &sample_param_2.a2_time,
									&sample_param_3.a2_time, &sample_param_4.a2_time};
	if(_addr < SAMPLE1_A2_TIME || _addr > SAMPLE4_A2_TIME)
		return RT_ERROR;
	*pa2_time[(_addr-SAMPLE1_A2_TIME)/2] = value;
	
	return RT_EOK;
}
static int sample_switch(rt_uint16_t _addr)
{
	rt_uint8_t* en_sample[4] = {&switch_config.en_sample_1, &switch_config.en_sample_2,
								&switch_config.en_sample_3, &switch_config.en_sample_4};
	
	if(_addr < SAMPLE_1_SWITCH || _addr > SAMPLE_4_SWITCH)
		return RT_ERROR;
	rt_uint8_t num =  _addr - SAMPLE_1_SWITCH;
	if(*en_sample[num])
	{
			*en_sample[num] = 0;
			ScreenDisICON(SAMPLE1_SWITCH_IOC + num, 0);
			work_cancel(0x10 << num);
	}
	else
	{
			ScreenDisICON(SAMPLE1_SWITCH_IOC + num, 1);
			work_create(0x01 << num);
			*en_sample[num] = 1;
	}
	return RT_EOK;
}
static int light_switch(rt_uint16_t _addr)
{
	static rt_uint8_t en_led[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	rt_uint8_t led_gpio[8] = {LED1_gpio, LED2_gpio, LED3_gpio, LED4_gpio,
								LED5_gpio, LED6_gpio, LED7_gpio, LED8_gpio};
	
	if(_addr < LIGHT_SWITCH_1 || _addr > LIGHT_SWITCH_8)
		return RT_ERROR;
	rt_uint8_t num =  _addr - LIGHT_SWITCH_1;
	if(en_led[num])
	{
		en_led[num] = 0;
		ScreenDisICON(LED1_SWITCH_ICO + num, 0);
		rt_pin_write(led_gpio[num], 0);
	}
	else
	{
		en_led[num] = 1;
		ScreenDisICON(LED1_SWITCH_ICO + num, 1);
		rt_pin_write(led_gpio[num], 1);
	}
	return RT_EOK;
}
static int blender_switch(rt_uint16_t _addr)
{
	static rt_uint8_t en_dcmotor[4] = {0, 0, 0, 0};	
	rt_uint8_t blender_gpio[4] = {blender1_gpio, blender2_gpio, blender3_gpio, blender4_gpio};
	
	if(_addr < DCMOTOR_SWITCH_1 || _addr > DCMOTOR_SWITCH_4)
		return RT_ERROR;
	rt_uint8_t num =  _addr - DCMOTOR_SWITCH_1;
	if(en_dcmotor[num])
	{
		en_dcmotor[num] = 0;
		rt_pin_write(blender_gpio[num], 1); 			
		ScreenDisICON(BLENDER1_START_ICO + num, 0);
		ScreenDisICON(BLENDER1_ICO + num, 0);
	}
	else
	{
		en_dcmotor[num] = 1;
		rt_pin_write(blender_gpio[num], 0);
		ScreenDisICON(BLENDER1_START_ICO + num, 1);
		ScreenDisICON(BLENDER1_ICO + num, 1);
	}
	return RT_EOK;
}
/**
 * @brief  处理屏幕按键
 *  
 * @param  keyval ：按键值
 *
 * @return
 */
void LcdKeyValDeal(rt_uint16_t keyval)
{	
	char string[30] = {0};
	rt_uint8_t _ret = 0;
	
	switch (keyval)
	{
	case SAMPLE:
		{
			rt_uint8_t* en_sample[4] = {&switch_config.en_sample_1, &switch_config.en_sample_2,
										&switch_config.en_sample_3, &switch_config.en_sample_4};
			rt_uint16_t switch_ico[4] = {SAMPLE1_SWITCH_IOC, SAMPLE2_SWITCH_IOC, SAMPLE3_SWITCH_IOC, SAMPLE4_SWITCH_IOC};
			rt_uint16_t status_ico[4] = {SAMPLE_1_ICO, SAMPLE_2_ICO, SAMPLE_3_ICO, SAMPLE_4_ICO};

			for(rt_uint8_t i = 0; i<4; i++)
			{
				if(*en_sample[i])	
				{					
					ScreenDisICON(switch_ico[i], 1);	
					ScreenDisICON(status_ico[i], 1);
				}
				else
				{
					ScreenDisICON(switch_ico[i], 0);	
					ScreenDisICON(status_ico[i], 0);					
				}					
			}
			parameter_display(&sample_param_1);
			parameter_display(&sample_param_4);
		}
		break;
	case QUALITY:
		rt_kprintf(" QUALITY\n");
		break;
	case FIND:
		rt_kprintf(" FIND\n");
		break;
	case SYSTEM:
		rt_kprintf(" SYSTEM\n");
		break;
	case BACK_MANU:
		switch_config.en_Light_1 = 0;
		switch_config.en_Temp_1 = 0;
		rt_kprintf(" BACK_MANU\n");
		break;
	case SAMPLE_CHANNEL_1:
	case SAMPLE_CHANNEL_2:
	case SAMPLE_CHANNEL_3:
	case SAMPLE_CHANNEL_4:
		sample_start(keyval);
		break;
	case SAMPLE_START:		
		_ret = work_create(switch_config.sample_start);
		switch_config.sample_start = 0;
		rt_kprintf(" SAMPLE_OK \n");
		break;
	case SAMPLE_STOP:		
		rt_kprintf(" SAMPLE_STOP \n");
		break;
	case SAMPLE_1_SWITCH:		
	case SAMPLE_2_SWITCH:
	case SAMPLE_3_SWITCH:
	case SAMPLE_4_SWITCH:		
		sample_switch(keyval);
		break;
	case RFID:
		rt_kprintf(" RFID\n");
	
		break;
	case LIGHT:		
		switch_config.en_Light_1 = 1;
		break;
	case TEMPERATURE:
		{
			rt_uint16_t tt = 0;
			switch_config.en_Temp_1 = 1;
			tt = (rt_uint16_t)HeatHandle_1.iSetVal;
			ScreenSendData(TEMP_SET, (rt_uint8_t*)&tt, 2);	
			tt = (rt_uint16_t)HeatHandle_1.CycleTime;
			ScreenSendData(TEMP_TIME, (rt_uint8_t*)&tt, 2);	
			tt = (rt_uint16_t)(HeatHandle_1.PID.uKP_Coe * 100);
			ScreenSendData(TEMP_KP, (rt_uint8_t*)&tt, 2);	
			tt = (rt_uint16_t)(HeatHandle_1.PID.uKI_Coe * 100);
			ScreenSendData(TEMP_KI, (rt_uint8_t*)&tt, 2);	
			tt = (rt_uint16_t)(HeatHandle_1.PID.uKD_Coe * 100);
			ScreenSendData(TEMP_KD, (rt_uint8_t*)&tt, 2);	
		}
		break;
	case DC_MOTOR:
		break;
	case SET_TIME:
		_ret = 0x06;
		ScreenSendCommand(READ_81, 0x20, &_ret, 1);
		break;
	case PRINTER:
		rt_kprintf(" PRINTER\n");
		break;
	case SERVER:
		rt_kprintf(" SERVER\n");
		break;
	case RFID_OK:
		rt_kprintf(" RFID_OK\n");
		break;
	case RFID_CLEAR:
		rt_kprintf(" RFID_CLEAR\n");
		break;
	case LIGHT_SWITCH_1:	
	case LIGHT_SWITCH_2:
	case LIGHT_SWITCH_3:
	case LIGHT_SWITCH_4:
	case LIGHT_SWITCH_5:
	case LIGHT_SWITCH_6:
	case LIGHT_SWITCH_7:
	case LIGHT_SWITCH_8:
		light_switch(keyval);
		break;
	case TEMP_OK:
		if(UartBuff.HeatSetBuf.pHeatTemp)
		{
			ScreenDisICON(UartBuff.HeatSetBuf.Ioc, 1);
			start_heat(UartBuff.HeatSetBuf.pHeatTemp);	
			switch_config.en_Heat_1 = 1;
			rt_memset(string, 0, sizeof(string));
			ScreenSendCommand(WRITE_82, TEMP_INFO, (rt_uint8_t*)string,sizeof(string));
		}
		else
		{
			rt_sprintf(string, "请选择通道！");
			ScreenSendCommand(WRITE_82, TEMP_INFO, (rt_uint8_t*)string,rt_strlen(string));
		}
		break;
	case TEMP_STOP:
		if(UartBuff.HeatSetBuf.pHeatTemp)
		{
			ScreenDisICON(UartBuff.HeatSetBuf.Ioc, 0);
			stop_heat(UartBuff.HeatSetBuf.pHeatTemp);	
			switch_config.en_Heat_1 = 0;
			rt_memset(string, 0, sizeof(string));
			ScreenSendCommand(WRITE_82, TEMP_INFO, (rt_uint8_t*)string,sizeof(string));
		}
		else
		{
			rt_sprintf(string, "请选择通道！");
			ScreenSendCommand(WRITE_82, TEMP_INFO, (rt_uint8_t*)string,rt_strlen(string));
		}
		break;
	case STEPMOTOR_START:
		if(UartBuff.MotorPara.h_Motor)
		{
			rt_memset(string, 0, sizeof(string));
			ScreenSendCommand(WRITE_82, MOTOR_INFO, (rt_uint8_t*)string,sizeof(string));
			_ret = StepMotor_AxisMoveRel(UartBuff.MotorPara.h_Motor, 
										 UartBuff.MotorPara.position, 
										 UartBuff.MotorPara.acceltime, 
										 UartBuff.MotorPara.deceltime,
										 UartBuff.MotorPara.speed);		
			if(_ret != 0)
				rt_kprintf(" error %d\n",_ret);
		}
		else
		{
			rt_sprintf(string, "请选择通道！");
			ScreenSendCommand(WRITE_82, MOTOR_INFO, (rt_uint8_t*)string,rt_strlen(string));
		}
		break;
	case STEPMOTOR_ZERO:
		if(UartBuff.MotorPara.h_Motor)
		{
			rt_memset(string, 0, sizeof(string));
			ScreenSendCommand(WRITE_82, MOTOR_INFO, (rt_uint8_t*)string,sizeof(string));
			//_ret = StepMotor_AxisMoveRel(UartBuff.MotorPara.h_Motor, 0, 0, 0, 2000);
			_ret = stepmotor_backzero(UartBuff.MotorPara.channel);
			if(_ret != 0)
				rt_kprintf(" error %d\n",_ret);
		}
		else
		{
			rt_sprintf(string, "请选择通道！");
			ScreenSendCommand(WRITE_82, MOTOR_INFO, (rt_uint8_t*)string,rt_strlen(string));
		}
		break;
	case STEPMOTOR_STOP:
		//StopMotorTimer(&Motor4);
		break;
	case tmc5130_START:
		tmc5130_moveToAbsolutePosition(UartBuff.MotorPara.tmc5230_position, 0);
		break;
	case tmc5130_STOP:
		rt_kprintf(" tmc5130_STOP\n");
		break;
	case tmc5130_ZERO:
		rt_kprintf(" tmc5130_ZERO\n");
		break;
	case DOOR_OPEN_1://左门开
		rt_pin_write(Ldoor_en_gpio, 1);
		rt_pin_write(Ldoormotor1_gpio, 1); 
		rt_thread_delay(3000); 
		rt_pin_write(Ldoormotor1_gpio, 0);
		rt_pin_write(Ldoor_en_gpio, 0);
		ScreenDisICON(LDOOR_ICO, 0);
		break;
	case DOOR_CLOSE_1://左门关
		rt_pin_write(Ldoor_en_gpio, 1);
		rt_pin_write(Ldoormotor2_gpio, 1); 
		rt_thread_delay(3000); 
		rt_pin_write(Ldoormotor2_gpio, 0); 
		rt_pin_write(Ldoor_en_gpio, 0);
		ScreenDisICON(LDOOR_ICO, 1);
		break;
	case DOOR_OPEN_2://右门开
		rt_pin_write(Rdoor_en_gpio, 1);
		rt_pin_write(Rdoormotor1_gpio, 1); 
		rt_thread_delay(3000); 
		rt_pin_write(Rdoormotor1_gpio, 0);
		rt_pin_write(Rdoor_en_gpio, 0);
		ScreenDisICON(RDOOR_ICO, 0);
		break;
	case DOOR_CLOSE_2://右门关
		rt_pin_write(Rdoor_en_gpio, 1);
		rt_pin_write(Rdoormotor2_gpio, 1); 
		rt_thread_delay(3000); 
		rt_pin_write(Rdoormotor2_gpio, 0);
		rt_pin_write(Rdoor_en_gpio, 0);
		ScreenDisICON(RDOOR_ICO, 1);
		break;
	case DCMOTOR_SWITCH_1://搅拌1
	case DCMOTOR_SWITCH_2://搅拌2
	case DCMOTOR_SWITCH_3://搅拌3
	case DCMOTOR_SWITCH_4://搅拌4
		blender_switch(keyval);
		break;
	case CALI_OK:
		rt_kprintf(" CALI_OK\n");
		break;
	case SET_TIME_OK:
		rt_kprintf(" SET_TIME_OK\n");
		break;
	case SET_TIME_CANCEL:
		rt_kprintf(" SET_TIME_CANCEL\n");
		break;
	case PRINTER_OK:
		printer_set_font(UartBuff.printer.font_size);
		printer_set_gap(UartBuff.printer.gap_size);
		break;
	case PRINTER_DEFAULT:
		printer_init();
		break;
	case PRINTER_FORMAT_1:
		TestPrinter();
		break;
	case PRINTER_FORMAT_2:
		break;
	case PRINTER_FORMAT_3:
		break;
	case KEY_SERVER_ACTIVATE:		
		//_ret = device_updata(&send_server_data, &rev_server_data);
		_ret = device_activate(&send_server_data, &rev_server_data);
		rt_memset(string, 0, 30);
		ScreenSendCommand(WRITE_82, SERVER_INFO, (rt_uint8_t*)string,30);
		if(RT_EOK == _ret)
		{
			
			rt_sprintf(string, "服务器激活成功！");
			ScreenSendCommand(WRITE_82, SERVER_INFO, (rt_uint8_t*)string,rt_strlen(string));
		}
		else
		{
			rt_sprintf(string, "服务器激活失败，错误代码：%d.",_ret);
			ScreenSendCommand(WRITE_82, SERVER_INFO, (rt_uint8_t*)string,rt_strlen(string));
		}
		break;
	case KEY_SERVER_TEST:
		_ret = device_test(&send_server_data, &rev_server_data);
		rt_memset(string, 0, 30);
		ScreenSendCommand(WRITE_82, SERVER_INFO, (rt_uint8_t*)string,30);
		if(RT_EOK == _ret)
		{
			rt_sprintf(string, "服务器测试成功！");
			ScreenSendCommand(WRITE_82, SERVER_INFO, (rt_uint8_t*)string,rt_strlen(string));
		}
		else
		{
			rt_sprintf(string, "服务器测试失败，错误代码：%d.",_ret);
			ScreenSendCommand(WRITE_82, SERVER_INFO, (rt_uint8_t*)string,rt_strlen(string));
		}
		break;
	case KEY_SERVER_UPLOAD:	
		_ret = device_sample(&send_server_data, &server_sample);
		rt_memset(string, 0, 30);
		ScreenSendCommand(WRITE_82, SERVER_INFO, (rt_uint8_t*)string,30);
		if(RT_EOK == _ret)
		{
			rt_sprintf(string, "上传数据服务器成功！");
			ScreenSendCommand(WRITE_82, SERVER_INFO, (rt_uint8_t*)string,rt_strlen(string));
		}
		else
		{
			rt_sprintf(string, "上传数据失败，错误代码：%d.",_ret);
			ScreenSendCommand(WRITE_82, SERVER_INFO, (rt_uint8_t*)string,rt_strlen(string));
		}
		break;
	case KEY_SERVER_SET:
		rt_kprintf("ip : %s\r\n",UartBuff.ServerSetBuf._ip);
		rt_kprintf("port : %s\r\n",UartBuff.ServerSetBuf._port);
		break;
	default:
		break;
	}
}
/**
 * 十六进制转十进制
 */
static rt_uint8_t h2d(rt_uint8_t _hex)
{
	rt_uint8_t remainder = (_hex&0xf0) >> 1;
	if(remainder)
		remainder += 2*((_hex&0xf0) >> 4);
	else
		remainder = 0;
	return remainder + (_hex&0x0f);
}
/**
 * 十进制转十六进制
 */
static rt_uint8_t d2h(rt_uint8_t _dec)
{
	rt_uint8_t remainder = (_dec >> 4);
	if(remainder)
		remainder += 2*((_dec&0xf0) >> 4);
	else
		remainder = 0;
	return remainder + (_dec&0x0f);
}
/**
 * @brief  
 *  
 * @param 
 *
 * @return
 */
void DealCmd(const rt_uint8_t* _ucData)
{
	rt_uint16_t _addr = 0;
	rt_uint16_t _data = 0;
	rt_uint8_t _rev = 0;
	char string[22] = {0};
	
	if(0x83 == _ucData[0])
	{
		_addr = _ucData[1]<<8 | _ucData[2];
		_data = _ucData[4]<<8 | _ucData[5];		
		//rt_kprintf(" rev data:%d\n",_data);
		switch (_addr)
        {
		case KEY_RETURN:			
			LcdKeyValDeal(_data);
			break;
		case SAMPLE1_HEAT_TIME:
		case SAMPLE2_HEAT_TIME:
		case SAMPLE3_HEAT_TIME:
		case SAMPLE4_HEAT_TIME:
			set_work_heat(_addr, _data);
			break;
		case SAMPLE1_READ_TIME:
		case SAMPLE2_READ_TIME:
		case SAMPLE3_READ_TIME:
		case SAMPLE4_READ_TIME:
			set_work_read(_addr, _data);
			break;
		case SAMPLE1_A1_TIME:
		case SAMPLE2_A1_TIME:
		case SAMPLE3_A1_TIME:
		case SAMPLE4_A1_TIME:
			set_work_a1(_addr, _data);
			break;
		case SAMPLE1_A2_TIME:
		case SAMPLE2_A2_TIME:
		case SAMPLE3_A2_TIME:
		case SAMPLE4_A2_TIME:
			set_work_a2(_addr, _data);
			break;
		case RESULT_SLIDER:			
			rt_kprintf("RESULT_SLIDER %d\n", _data);
			break;
		case RFID_SET:
			break;
		case TEMP_CHANNEL:
			_rev = choose_channel(TEMP_CHANNEL, _data);	
			rt_memset(string, 0, sizeof(string));
			ScreenSendCommand(WRITE_82, TEMP_INFO, (rt_uint8_t*)string,sizeof(string));
			switch (_rev)
            {
			case 1:
				UartBuff.HeatSetBuf.pHeatTemp = &HeatHandle_1;
				UartBuff.HeatSetBuf.Ioc = TEMP1_ICO;
				break;
			case 2:
				break;
			default:
				UartBuff.HeatSetBuf.pHeatTemp = RT_NULL;
				break;
            }
			break;
		case TEMP_SET:
			if(UartBuff.HeatSetBuf.pHeatTemp)
			{
				UartBuff.HeatSetBuf.pHeatTemp->iSetVal = _data * 10;
				rt_kprintf(" TEMP_SET:%d\n",_data);
				rt_memset(string, 0, sizeof(string));
				ScreenSendCommand(WRITE_82, TEMP_INFO, (rt_uint8_t*)string,sizeof(string));
			}
			else
			{
				rt_sprintf(string, "请选择通道！");
				ScreenSendCommand(WRITE_82, TEMP_INFO, (rt_uint8_t*)string,rt_strlen(string));
			}
			break;
		case TEMP_TIME:
			if(UartBuff.HeatSetBuf.pHeatTemp)
			{
				UartBuff.HeatSetBuf.pHeatTemp->CycleTime = _data;
				rt_memset(string, 0, sizeof(string));
				ScreenSendCommand(WRITE_82, TEMP_INFO, (rt_uint8_t*)string,sizeof(string));
			}
			else
			{
				rt_sprintf(string, "请选择通道！");
				ScreenSendCommand(WRITE_82, TEMP_INFO, (rt_uint8_t*)string,rt_strlen(string));
			}
			break;
		case TEMP_KP:		
			if(UartBuff.HeatSetBuf.pHeatTemp)
			{
				UartBuff.HeatSetBuf.pHeatTemp->PID.uKP_Coe = (float)_data / 100;
				rt_memset(string, 0, sizeof(string));
				ScreenSendCommand(WRITE_82, TEMP_INFO, (rt_uint8_t*)string,sizeof(string));
			}
			else
			{
				rt_sprintf(string, "请选择通道！");
				ScreenSendCommand(WRITE_82, TEMP_INFO, (rt_uint8_t*)string,rt_strlen(string));
			}
			break;
		case TEMP_KI:			
			if(UartBuff.HeatSetBuf.pHeatTemp)
			{
				UartBuff.HeatSetBuf.pHeatTemp->PID.uKP_Coe = (float)_data / 100;
				rt_memset(string, 0, sizeof(string));
				ScreenSendCommand(WRITE_82, TEMP_INFO, (rt_uint8_t*)string,sizeof(string));
			}
			else
			{
				rt_sprintf(string, "请选择通道！");
				ScreenSendCommand(WRITE_82, TEMP_INFO, (rt_uint8_t*)string,rt_strlen(string));
			}
			break;
		case TEMP_KD:			
			if(UartBuff.HeatSetBuf.pHeatTemp)
			{
				UartBuff.HeatSetBuf.pHeatTemp->PID.uKP_Coe = (float)_data / 100;
				rt_memset(string, 0, sizeof(string));
				ScreenSendCommand(WRITE_82, TEMP_INFO, (rt_uint8_t*)string,sizeof(string));
			}
			else
			{
				rt_sprintf(string, "请选择通道！");
				ScreenSendCommand(WRITE_82, TEMP_INFO, (rt_uint8_t*)string,rt_strlen(string));
			}
			break;
		case TIME_YEAR:
			UartBuff.RealTime.year = _data;
			break;
		case TIME_MONTH:
			UartBuff.RealTime.month = _data;
			break;
		case TIME_DAY:
			UartBuff.RealTime.day = _data;
			break;
		case TIME_HOUR:
			UartBuff.RealTime.hour = _data;
			break;
		case TIME_MINUTE:
			UartBuff.RealTime.minute = _data;
			break;
		case PRINTER_FONT:
			UartBuff.printer.font_size = _data;
			break;
		case PRINTER_DISTANCE:
			UartBuff.printer.gap_size = _data;
			break;
		case BLENDER_TIME1:
			break;
		case BLENDER_TIME2:
			break;
		case MOTOR_CHANNEL:
			UartBuff.MotorPara.channel = _data;
			_rev = choose_channel(MOTOR_CHANNEL, _data);	
			rt_memset(string, 0, sizeof(string));
			ScreenSendCommand(WRITE_82, MOTOR_INFO, (rt_uint8_t*)string,sizeof(string));
			switch (_rev)
            {
			case 1:
				UartBuff.MotorPara.h_Motor = &Motor1;
				//UartBuff.HeatSetBuf.Ioc = TEMP1_IOC;
				break;
			case 2:
				UartBuff.MotorPara.h_Motor = &Motor2;
				break;
			case 3:
				UartBuff.MotorPara.h_Motor = &Motor3;
				//UartBuff.HeatSetBuf.Ioc = TEMP1_IOC;
				break;
			case 4:
				UartBuff.MotorPara.h_Motor = &Motor4;
				break;
			default:
				UartBuff.MotorPara.h_Motor = RT_NULL;
				break;
            }		
			break;
		case MOTOR_POSITION:			
			if(UartBuff.MotorPara.h_Motor)
			{
				if(0 == _data)
					_data++;
				UartBuff.MotorPara.position = _data;
				rt_memset(string, 0, sizeof(string));
				ScreenSendCommand(WRITE_82, MOTOR_INFO, (rt_uint8_t*)string,sizeof(string));
			}
			else
			{
				rt_sprintf(string, "请选择通道！");
				ScreenSendCommand(WRITE_82, MOTOR_INFO, (rt_uint8_t*)string,rt_strlen(string));
			}
			break;
		case MOTOR_SPEED:
			if(UartBuff.MotorPara.h_Motor)
			{
				UartBuff.MotorPara.speed = _data;
				rt_memset(string, 0, sizeof(string));
				ScreenSendCommand(WRITE_82, MOTOR_INFO, (rt_uint8_t*)string,sizeof(string));
			}
			else
			{
				rt_sprintf(string, "请选择通道！");
				ScreenSendCommand(WRITE_82, MOTOR_INFO, (rt_uint8_t*)string,rt_strlen(string));
			}
			break;
		case MOTOR_ACCELTIME:
			if(UartBuff.MotorPara.h_Motor)
			{
				UartBuff.MotorPara.acceltime = _data;
				rt_memset(string, 0, sizeof(string));
				ScreenSendCommand(WRITE_82, MOTOR_INFO, (rt_uint8_t*)string,sizeof(string));
			}
			else
			{
				rt_sprintf(string, "请选择通道！");
				ScreenSendCommand(WRITE_82, MOTOR_INFO, (rt_uint8_t*)string,rt_strlen(string));
			}
			break;
		case MOTOR_DECELTIME:
			if(UartBuff.MotorPara.h_Motor)
			{
				UartBuff.MotorPara.deceltime = _data;
				rt_memset(string, 0, sizeof(string));
				ScreenSendCommand(WRITE_82, MOTOR_INFO, (rt_uint8_t*)string,sizeof(string));
			}
			else
			{
				rt_sprintf(string, "请选择通道！");
				ScreenSendCommand(WRITE_82, MOTOR_INFO, (rt_uint8_t*)string,rt_strlen(string));
			}
			break;
/**************测试5130******************/			
		case tmc5130_POSITION:
			UartBuff.MotorPara.tmc5230_position = _data*1000;
			break;
		case tmc5130_A1:
			rt_kprintf("%d\n",_data);
			tmc5130_writeInt(TMC5130_A1, _data*1000);
			break;
		case tmc5130_AMAX:
			rt_kprintf("%d\n",_data);					
			tmc5130_writeInt(TMC5130_AMAX, _data*1000);
			break;
		case tmc5130_D1:
			tmc5130_writeInt(TMC5130_D1, _data*1000);			
			break;
		case tmc5130_DMAX:
			tmc5130_writeInt(TMC5130_DMAX, _data*1000);
			break;
		case tmc5130_V1:
			UartBuff.MotorPara.tmc5230_v1 = _data*1000;
			tmc5130_writeInt(TMC5130_V1, UartBuff.MotorPara.tmc5230_v1);
			break;
		case tmc5130_VMAX:		
			UartBuff.MotorPara.tmc5230_vmax = _data*1000;
			tmc5130_writeInt(TMC5130_VMAX, UartBuff.MotorPara.tmc5230_vmax);
			break;
		case tmc5130_VSTART:
			tmc5130_writeInt(TMC5130_VSTART, _data*1000);
			break;
		case tmc5130_VSTOP:
			tmc5130_writeInt(TMC5130_VSTOP, _data*1000);
			break;
		case tmc5130_IHOLD:
			tmc5130_setIHold(_data);
			break;
		case tmc5130_IRUN:
			tmc5130_setIRun(_data);
			break;
/**************测试5130******************/		
		case SERVER_IP:
			{
				rt_uint8_t ip_len = (_ucData[3]-1) * 2;
				rt_memcpy(UartBuff.ServerSetBuf._ip, &_ucData[4], ip_len);
				if((rt_uint8_t)UartBuff.ServerSetBuf._ip[ip_len-1] == 0xff)
					UartBuff.ServerSetBuf._ip[ip_len-1] = 0;
			}
			break;
		case SERVER_PORT:
			{
				rt_uint8_t port_len = (_ucData[3]-1) * 2;
				rt_memcpy(UartBuff.ServerSetBuf._port, &_ucData[4], port_len);
				if((rt_uint8_t)UartBuff.ServerSetBuf._port[port_len-1] == 0xff)
					UartBuff.ServerSetBuf._port[port_len-1] = 0;
			}
			break;
		default:
			break;
        }
	}
	else if(0x81 == _ucData[0])
	{
		if(0x20 == _ucData[1])
		{
			UartBuff.RealTime.year = _ucData[3];
			ScreenSendData_2bytes(TIME_YEAR, h2d(UartBuff.RealTime.year)+2000);
			UartBuff.RealTime.month = _ucData[4];
			ScreenSendData_2bytes(TIME_MONTH, h2d(UartBuff.RealTime.month));
			UartBuff.RealTime.day = _ucData[5];
			ScreenSendData_2bytes(TIME_DAY, h2d(UartBuff.RealTime.day));
			UartBuff.RealTime.hour = _ucData[7];
			ScreenSendData_2bytes(TIME_HOUR, h2d(UartBuff.RealTime.hour));
			UartBuff.RealTime.minute = _ucData[8];
			ScreenSendData_2bytes(TIME_MINUTE, h2d(UartBuff.RealTime.minute));
		}
	}
}



/**
 * @brief  
 *  
 * @param 
 *
 * @return
 */
void GetUartScreenCMD(rt_uint8_t _reData)
{	
	static uint8_t pos = 0;
	static uint8_t len = 0;
	switch (len)
    {
    	case 0:
			if(0xa5 == _reData)   //接收帧头
				len ++;
			else
				len = 0;
			break;
		case 1:
			if(0x5a == _reData)   //接收帧头
				len ++;
			else
				len = 0;
    		break;
    	case 2:
			UartBuff.rxCount = _reData;
			if(UartBuff.rxCount > 20)			
				len = 0;			
			else
				len ++;
    		break;
		case 3:			
			UartBuff.rxBuff[pos++] = _reData;
			if(pos >= UartBuff.rxCount)
			{
				len = 0;
				pos = 0;
				DealCmd(UartBuff.rxBuff);
			}
    		break;
    	default:
    		break;
    }

}
/**
 * @brief  显示字符串和发送控制命令
 *  
 * @param 
 *
 * @return
 */
rt_uint8_t ScreenSendCommand(rt_uint8_t _cmd, rt_uint16_t s_addr, rt_uint8_t* str, rt_uint8_t len)
{
	rt_uint8_t* sendBuf = NULL;
	rt_uint8_t i = 0;
	rt_uint8_t sendlen = 0;
		
	sendlen = len + 6;
	sendBuf = rt_malloc(sendlen);
	if(RT_NULL == sendBuf)
		return RT_ERROR;
	sendBuf[i++] = 0xa5;
	sendBuf[i++] = 0x5a;				/* 頭帧 */
	i++;
	sendBuf[i++] = _cmd;				/* 指令 */

	if(WRITE_82 == _cmd)
	{
		sendBuf[i++] = (rt_uint8_t)(s_addr>>8 & 0x00ff);
		sendBuf[i++] = (rt_uint8_t)(s_addr & 0x00ff);/* 地址 */
	}
	else
	{
		sendBuf[i++] = (rt_uint8_t)(s_addr & 0x00ff);/* 地址 */
		sendlen -= 1; 
	}	
	rt_memcpy(&sendBuf[i++], str, len);
	sendBuf[2] = sendlen - 3;			/* 长度 */
	for(i = 0;i < sendlen;i++)
	{
		uart_putchar(sendBuf[i]);
    }

	if(sendBuf)
		rt_free(sendBuf);
	return RT_EOK;
}

/*
 * 显示数据变量
 */
rt_uint8_t ScreenSendData(rt_uint16_t s_addr, rt_uint8_t* str, rt_uint8_t len)
{
	rt_uint8_t* sendBuf = NULL;
	rt_uint8_t i = 0;
	rt_uint8_t sendlen = 0;
		
	sendlen = len + 6;
	sendBuf = rt_malloc(sendlen);
	if(RT_NULL == sendBuf)
		return RT_ERROR;
	sendBuf[i++] = 0xa5;
	sendBuf[i++] = 0x5a;		/* 頭帧 */
	i++;
	sendBuf[i++] = WRITE_82;	/* 指令 */

	sendBuf[i++] = (rt_uint8_t)(s_addr>>8 & 0x00ff);
	sendBuf[i++] = (rt_uint8_t)(s_addr & 0x00ff);/* 地址 */
	
	while(len > 0)
	{
		sendBuf[i++] = str[len-1];
		len--;
	}
	sendBuf[2] = sendlen - 3;			/* 长度 */
	for(i = 0;i < sendlen;i++)
	{
		uart_putchar(sendBuf[i]);
    }

	if(sendBuf)
		rt_free(sendBuf);
	return RT_EOK;
}
/*
 * 显示数据变量(两个字节)
 */
rt_uint8_t ScreenSendData_2bytes(rt_uint16_t s_addr, rt_uint16_t _data)
{
	rt_uint8_t sendBuf[8] = {0};
	rt_uint8_t i = 0;
		
	sendBuf[i++] = 0xa5;
	sendBuf[i++] = 0x5a;				/* 頭帧 */
	sendBuf[i++] = 5;					/* 长度 */
	sendBuf[i++] = WRITE_82;			/* 指令 */

	sendBuf[i++] = (rt_uint8_t)(s_addr>>8 & 0x00ff);
	sendBuf[i++] = (rt_uint8_t)(s_addr & 0x00ff);		/* 地址 */

	sendBuf[i++] = (rt_uint8_t)(_data>>8 & 0x00ff);
	sendBuf[i++] = (rt_uint8_t)(_data & 0x00ff);		/* 数据 */	
	
	for(i = 0;i < sizeof(sendBuf);i++)
	{
		uart_putchar(sendBuf[i]);
    }
	
	return RT_EOK;
}
/*
 * 显示图标变量
 */
rt_uint8_t ScreenDisICON(rt_uint16_t s_addr, rt_uint8_t status)
{
	rt_uint8_t _temp[2] = {0, status};
	ScreenSendCommand(WRITE_82, s_addr, _temp, 2);
	
	return RT_EOK;
}

rt_err_t uart_open(const char *name)
{
    rt_err_t res;

    /* 查找系统中的串口设备 */
    uart_device = rt_device_find(name);   
    /* 查找到设备后将其打开 */
    if (uart_device != RT_NULL)
    {          
        res = rt_device_set_rx_indicate(uart_device, uart_intput);
        /* 检查返回值 */
        if (res != RT_EOK)
        {
            rt_kprintf("set %s rx indicate error.%d\n",name,res);
            return -RT_ERROR;
        }

        /* 打开设备，以可读写、中断方式 */
        res = rt_device_open(uart_device, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX );       
        /* 检查返回值 */
        if (res != RT_EOK)
        {
            rt_kprintf("open %s device error.%d\n",name,res);
            return -RT_ERROR;
        }

        /* 初始化事件对象 */
        rt_event_init(&event, "event", RT_IPC_FLAG_FIFO); 
    }
    else
    {
        rt_kprintf("can't find %s device.\n",name);
        return -RT_ERROR;
    }

    return RT_EOK;
}


void Function_UartScreen(void* parameter)
{
	rt_uint8_t uart_rx_data;
	uart_open(RT_SCREEN_DEVICE_NAME);
	
	while(1)
	{
		uart_rx_data = uart_getchar();
		GetUartScreenCMD(uart_rx_data);
		//rt_kprintf("%x\r\n", uart_rx_data);
		rt_thread_delay(10);
	}
}


