#include "Uart_Screen.h"
#include "RunLED.h"
#include "ServerData.h"
#include "WorkTask.h"
#include "bsp_RMD8.h"
#include "SenseData.h"
#include "TMC5130.h"
#include "dc_motor.h"

UartBuff_t UartBuff;

/* ���ڽ����¼���־ */
#define UART_RX_EVENT (1 << 0)

/* �¼����ƿ� */
static struct rt_event event;
/* �����豸��� */
static rt_device_t uart_device = RT_NULL;

static rt_uint8_t SetVD_Central(rt_uint16_t range);
static rt_uint8_t SetX_amp(rt_uint8_t _amp);

/* �ص����� */
static rt_err_t uart_intput(rt_device_t dev, rt_size_t size)
{
    /* �����¼� */
    rt_event_send(&event, UART_RX_EVENT);
    
    return RT_EOK;
}
 
rt_uint8_t uart_getchar(void)
{
    rt_uint32_t e;
    rt_uint8_t ch;

    /* ��ȡ1�ֽ����� */
    while (rt_device_read(uart_device, 0, &ch, 1) != 1)
    {
         /* �����¼� */
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
	char str[3] = {0};
	
	switch (_channel)
	{
	case 1:		
		rt_sprintf(str, "һ");
		ScreenSendCommand(WRITE_82, _addr, (rt_uint8_t*)str,rt_strlen(str));
		break;
	case 2:
		rt_sprintf(str, "��");
		ScreenSendCommand(WRITE_82, _addr, (rt_uint8_t*)str,rt_strlen(str));
		break;
	case 3:
		rt_sprintf(str, "��");
		ScreenSendCommand(WRITE_82, _addr, (rt_uint8_t*)str,rt_strlen(str));
		break;
	case 4:
		rt_sprintf(str, "��");
		ScreenSendCommand(WRITE_82, _addr, (rt_uint8_t*)str,rt_strlen(str));
		break;
	default:
		_channel = 0;
		break;
	}
	
	return _channel;
}
/**
 *  ������������
 */
static int sample_start(rt_uint16_t _addr)
{		
	char str[25] = {0};
	rt_uint8_t _rev;
	if(_addr < SAMPLE_SWITCH_1 || _addr > SAMPLE_SWITCH_4)
		return RT_ERROR;
	rt_uint8_t* sample_status[4] = {&status_config.flow1_status, &status_config.flow2_status,
										&status_config.flow3_status, &status_config.flow4_status};
	rt_uint8_t ch =  _addr - SAMPLE_SWITCH_1;
	rt_uint16_t LCD_info = SAMPLE1_INFO + ch * 32;

	switch (*sample_status[ch])
	{
	case FLOW_READY:
		ch = ch / 2 * 2;
		if(*sample_status[ch] == FLOW_RUN || *sample_status[ch + 1] == FLOW_RUN)
		{
			rt_sprintf(str, "�в�������������");
			ScreenSendCommand(WRITE_82, LCD_info, (rt_uint8_t*)str, sizeof(str));
			return RT_ERROR;
		}
		LCD_info = SAMPLE1_INFO + ch * 32;
		*sample_status[ch] = *sample_status[ch + 1] = FLOW_ADD;
		ScreenDisICON(SAMPLE1_SWITCH_IOC + ch, 2);	 //��ʾ����
		ScreenDisICON(SAMPLE1_SWITCH_IOC + ch + 1, 2);
		ScreenDisICON(SAMPLE1_PROG_IOC + ch, 0);
		ScreenDisICON(SAMPLE1_PROG_IOC + ch + 1, 0);
		rt_sprintf(str, "������Լ�����ʼ����");
		ScreenSendCommand(WRITE_82, LCD_info, (rt_uint8_t*)str, sizeof(str));
		ScreenSendCommand(WRITE_82, LCD_info + 32, (rt_uint8_t*)str, sizeof(str));
		door_start(ch/2, 1);
		break;
	case FLOW_ADD:
		door_start(ch/2, 0);
		*sample_status[ch] = FLOW_RUN;
		ScreenDisICON(SAMPLE1_SWITCH_IOC + ch, 1); //��ʾֹͣ
		rt_sprintf(str, "׼���С�����");
		ScreenSendCommand(WRITE_82, LCD_info, (rt_uint8_t*)str, sizeof(str));
		_rev = work_create(ch);	//��������
		if(_rev != RT_EOK)
			rt_kprintf("create work failed\n");
		break;
	case FLOW_RUN:
		*sample_status[ch] = FLOW_READY;
		rt_sprintf(str, "����ֹ�������¿�ʼ");
		ScreenSendCommand(WRITE_82, LCD_info, (rt_uint8_t*)str, sizeof(str));
		_rev = work_cancel(ch);	//ȡ������
		if(_rev != RT_EOK)
			rt_kprintf("cancel work failed\n");
		break;
	default:
		*sample_status[ch] = FLOW_READY;
		ScreenDisICON(SAMPLE1_SWITCH_IOC + ch, 0);
		rt_sprintf(str, "�����ʼ��Ȼ������Լ�");
		ScreenSendCommand(WRITE_82, LCD_info, (rt_uint8_t*)str,rt_strlen(str));
		break;
	}

	return RT_EOK;
}
/**
 *  ��ʾ״̬
 */
static int dis_sample_status(void)
{
	rt_uint8_t* sample_status[4] = {&status_config.flow1_status, &status_config.flow2_status,
										&status_config.flow3_status, &status_config.flow4_status};
	rt_uint16_t switch_ico[4] = {SAMPLE1_SWITCH_IOC, SAMPLE2_SWITCH_IOC, SAMPLE3_SWITCH_IOC, SAMPLE4_SWITCH_IOC};
	rt_uint16_t LCD_info[4] = {SAMPLE1_INFO, SAMPLE2_INFO, SAMPLE3_INFO, SAMPLE4_INFO};
	char str[25] = {0};

	for(rt_uint8_t i = 0; i<4; i++)
	{
		if(*sample_status[i] == FLOW_READY)
		{
			ScreenDisICON(switch_ico[i], 0);
			rt_sprintf(str, "�����ʼ��Ȼ������Լ�");
			ScreenSendCommand(WRITE_82, LCD_info[i], (rt_uint8_t*)str,rt_strlen(str));
		}
	}
}
/**
 *	�����������̲���
 */
static int set_sample_para(rt_uint8_t _ch)
{
	sample_param_t *psample_param[4] = {&sample_param_1, &sample_param_2,
											&sample_param_3, &sample_param_4};
	char str[12] = {0};

	if(_ch > 0 && _ch < 5)
	{
		psample_param[_ch-1]->mix_time = UartBuff.flow_para.mix_time;
		psample_param[_ch-1]->heat_time = UartBuff.flow_para.heat_time;
		psample_param[_ch-1]->read_time = UartBuff.flow_para.read_time;
		psample_param[_ch-1]->a1_time = UartBuff.flow_para.a1_time;
		psample_param[_ch-1]->a2_time = UartBuff.flow_para.a2_time;
		rt_memset(str, 0, sizeof(str));
		ScreenSendCommand(WRITE_82, TEMP_INFO, (rt_uint8_t*)str,sizeof(str));
	}
	else
	{
		rt_sprintf(str, "��ѡ��ͨ����");
		ScreenSendCommand(WRITE_82, TEMP_INFO, (rt_uint8_t*)str,rt_strlen(str));
	}

	return RT_EOK;
}
/**
 *	��ʾ���̲���
 */
static void flow_para_display(rt_uint8_t _ch)
{
	sample_param_t *psample_param[4] = {&sample_param_1, &sample_param_2,
											&sample_param_3, &sample_param_4};

	if(_ch > 0 && _ch < 5)
	{
		UartBuff.flow_para.mix_time = psample_param[_ch-1]->mix_time;
		UartBuff.flow_para.heat_time = psample_param[_ch-1]->heat_time;
		UartBuff.flow_para.read_time = psample_param[_ch-1]->read_time;
		UartBuff.flow_para.a1_time = psample_param[_ch-1]->a1_time;
		UartBuff.flow_para.a2_time = psample_param[_ch-1]->a2_time;
		ScreenSendData_2bytes(SAMPLE_MIX_TIME, psample_param[_ch-1]->mix_time);
		ScreenSendData_2bytes(SAMPLE_HEAT_TIME, psample_param[_ch-1]->heat_time);
		ScreenSendData_2bytes(SAMPLE_READ_TIME, psample_param[_ch-1]->read_time);
		ScreenSendData_2bytes(SAMPLE_A1_TIME, psample_param[_ch-1]->a1_time);
		ScreenSendData_2bytes(SAMPLE_A2_TIME, psample_param[_ch-1]->a2_time);
	}
}

/**
 *	ֱ��� ����
 */
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
/**
 *	������ ����
 */
static int blender_switch(rt_uint16_t _addr)
{
	static rt_uint8_t en_dcmotor[4] = {0, 0, 0, 0};	
	
	if(_addr < DCMOTOR_SWITCH_1 || _addr > DCMOTOR_SWITCH_4)
		return RT_ERROR;
	rt_uint8_t num =  _addr - DCMOTOR_SWITCH_1;
	if(en_dcmotor[num])
	{
		en_dcmotor[num] = 0;
		switch_blender(num, 0);
		ScreenDisICON(BLENDER1_START_ICO + num, 0);
		ScreenDisICON(BLENDER1_ICO + num, 0);
	}
	else
	{
		en_dcmotor[num] = 1;
		switch_blender(num, 1);
		ScreenDisICON(BLENDER1_START_ICO + num, 1);
		ScreenDisICON(BLENDER1_ICO + num, 1);
	}


	return RT_EOK;
}
/**
 *	����ϵͳ ����
 */
static int heat_switch(rt_uint8_t _ch, rt_uint8_t _config)
{
	struct HeatSystem_t* pHeatTemp[4] = {&HeatHandle_1, &HeatHandle_2,
										  &HeatHandle_3, &HeatHandle_4};
	rt_uint32_t temp_ico[4] = {TEMP1_ICO, TEMP2_ICO, TEMP3_ICO, TEMP4_ICO};
	rt_uint8_t* en_heat[4] = {&switch_config.en_Heat_1, &switch_config.en_Heat_2,
								&switch_config.en_Heat_3, &switch_config.en_Heat_4};
	char str[12] = {0};

	if(_ch)
	{
		ScreenDisICON(temp_ico[_ch-1], _config);
		heat_start_stop(pHeatTemp[_ch-1], _config);
		*en_heat[_ch-1] = _config;
		rt_memset(str, 0, sizeof(str));
		ScreenSendCommand(WRITE_82, TEMP_INFO, (rt_uint8_t*)str,sizeof(str));
	}
	else
	{
		rt_sprintf(str, "��ѡ��ͨ����");
		ScreenSendCommand(WRITE_82, TEMP_INFO, (rt_uint8_t*)str,rt_strlen(str));
	}

	return RT_EOK;
}
/**
 *	���ü���ϵͳ����
 */
static int set_heat_para(rt_uint8_t _ch)
{
	struct HeatSystem_t* pHeatTemp[4] = {&HeatHandle_1, &HeatHandle_2,
										  &HeatHandle_3, &HeatHandle_4};
	char str[12] = {0};

	if(_ch)
	{
		pHeatTemp[_ch-1]->iSetVal = UartBuff.heat_para.iSetVal;
		pHeatTemp[_ch-1]->CycleTime = UartBuff.heat_para.CycleTime;
		pHeatTemp[_ch-1]->PID.uKP_Coe = UartBuff.heat_para.uKP_Coe;
		pHeatTemp[_ch-1]->PID.uKI_Coe = UartBuff.heat_para.uKI_Coe;
		pHeatTemp[_ch-1]->PID.uKD_Coe = UartBuff.heat_para.uKD_Coe;

		rt_memset(str, 0, sizeof(str));
		ScreenSendCommand(WRITE_82, TEMP_INFO, (rt_uint8_t*)str,sizeof(str));
	}
	else
	{
		rt_sprintf(str, "��ѡ��ͨ����");
		ScreenSendCommand(WRITE_82, TEMP_INFO, (rt_uint8_t*)str,rt_strlen(str));
	}

	return RT_EOK;
}
/**
 *	��ʾ����ϵͳ����
 */
static int display_heat_para(rt_uint8_t _ch)
{
	struct HeatSystem_t* pHeatTemp[4] = {&HeatHandle_1, &HeatHandle_2,
										  &HeatHandle_3, &HeatHandle_4};
	rt_uint16_t tt = 0;

	if(_ch)
	{
		tt = (rt_uint16_t)pHeatTemp[_ch-1]->iSetVal;
		ScreenSendData_2bytes(TEMP_SET, tt);
		tt = (rt_uint16_t)pHeatTemp[_ch-1]->CycleTime;
		ScreenSendData_2bytes(TEMP_TIME, tt);
		tt = (rt_uint16_t)(pHeatTemp[_ch-1]->PID.uKP_Coe * 100);
		ScreenSendData_2bytes(TEMP_KP, tt);
		tt = (rt_uint16_t)(pHeatTemp[_ch-1]->PID.uKI_Coe * 100);
		ScreenSendData_2bytes(TEMP_KI, tt);
		tt = (rt_uint16_t)(pHeatTemp[_ch-1]->PID.uKD_Coe * 100);
		ScreenSendData_2bytes(TEMP_KD, tt);
	}

	return RT_EOK;
}
/**
 *	���ô�����ʱ��
 */
static int set_LCD_time(void)
{
	/*
	 * 5A A5 0A 80 1F 5A 15 07 03  00 16 05 59
                ������Ϊ2015��07��03��16��05��59���� �õ�00�Զ����㣩
	 *
	 * */
	rt_uint8_t wirte_buf[9] = {0x1f, 0x5a, 0, 0, 0, 0, 0, 0, 0};

	rt_memcpy(&wirte_buf[2], &UartBuff.RealTime.year, 3);
	rt_memcpy(&wirte_buf[6], &UartBuff.RealTime.hour, 2);

	ScreenSendCommand(WRITE_80, 0x20, wirte_buf, sizeof(wirte_buf));

	return RT_EOK;
}
/**
 *	���ô���������
 */
static int set_LCD_light(void)
{
	return RT_EOK;
}
/**
 * @brief  ������Ļ����
 *  
 * @param  keyval ������ֵ
 *
 * @return
 */
static void LcdKeyValDeal(rt_uint16_t keyval)
{	
	char string[30] = {0};
	rt_uint8_t _ret = 0;
	
	switch (keyval)
	{
	case SAMPLE:
		dis_sample_status();
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
		switch_config.en_Light_2 = 0;
		switch_config.en_Light_3 = 0;
		switch_config.en_Light_4 = 0;
		switch_config.en_Temp_1 = 0;
		switch_config.en_Temp_2 = 0;
		switch_config.en_Temp_3 = 0;
		switch_config.en_Temp_4 = 0;
		rt_kprintf(" BACK_MANU\n");
		break;
	case SAMPLE_SWITCH_1:
	case SAMPLE_SWITCH_2:
	case SAMPLE_SWITCH_3:
	case SAMPLE_SWITCH_4:
		sample_start(keyval);
		break;
	case SAMPLE_PARA_OK:
		set_sample_para(UartBuff.flow_para.channel);
		break;
	case SAMPLE_SET_PARA:

		break;
	case RFID:
		rt_kprintf(" RFID\n");
		break;
	case LIGHT:		
		switch_config.en_Light_1 = 1;
		switch_config.en_Light_2 = 1;
		switch_config.en_Light_3 = 1;
		switch_config.en_Light_4 = 1;
		break;
	case TEMPERATURE:
		switch_config.en_Temp_1 = 1;
		switch_config.en_Temp_2 = 1;
		switch_config.en_Temp_3 = 1;
		switch_config.en_Temp_4 = 1;

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
		set_heat_para(UartBuff.heat_para.channel);
		heat_switch(UartBuff.heat_para.channel, 1);
		break;
	case TEMP_STOP:
		heat_switch(UartBuff.heat_para.channel, 0);
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
			rt_sprintf(string, "��ѡ��ͨ����");
			ScreenSendCommand(WRITE_82, MOTOR_INFO, (rt_uint8_t*)string,rt_strlen(string));
		}
		break;
	case STEPMOTOR_ZERO:
		if(UartBuff.MotorPara.h_Motor)
		{
			rt_memset(string, 0, sizeof(string));
			ScreenSendCommand(WRITE_82, MOTOR_INFO, (rt_uint8_t*)string,sizeof(string));
			_ret = stepmotor_backzero(UartBuff.MotorPara.channel);
			if(_ret != 0)
				rt_kprintf(" error %d\n",_ret);
		}
		else
		{
			rt_sprintf(string, "��ѡ��ͨ����");
			ScreenSendCommand(WRITE_82, MOTOR_INFO, (rt_uint8_t*)string,rt_strlen(string));
		}
		break;
	case STEPMOTOR_STOP:
		//StopMotorTimer(&Motor4);
		break;
	case tmc5130_START:
		//tmc5130_moveToAbsolutePosition(UartBuff.MotorPara.tmc5230_position, 0);
		break;
	case tmc5130_STOP:
		rt_kprintf(" tmc5130_STOP\n");
		break;
	case tmc5130_ZERO:
		rt_kprintf(" tmc5130_ZERO\n");
		break;
	case DOOR_OPEN_1://���ſ�
		door_start(0, DOOR_OPEN);
		break;
	case DOOR_CLOSE_1://���Ź�
		door_start(0, DOOR_CLOSE);
		break;
	case DOOR_OPEN_2://���ſ�
		door_start(1, DOOR_OPEN);
		break;
	case DOOR_CLOSE_2://���Ź�
		door_start(1, DOOR_CLOSE);
		break;
	case DCMOTOR_SWITCH_1://����1
	case DCMOTOR_SWITCH_2://����2
	case DCMOTOR_SWITCH_3://����3
	case DCMOTOR_SWITCH_4://����4
		blender_switch(keyval);
		break;
	case CALI_OK:
		rt_kprintf(" CALI_OK\n");
		break;
	case SET_TIME_OK:
		set_LCD_time();
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
			
			rt_sprintf(string, "����������ɹ���");
			ScreenSendCommand(WRITE_82, SERVER_INFO, (rt_uint8_t*)string,rt_strlen(string));
		}
		else
		{
			rt_sprintf(string, "����������ʧ�ܣ�������룺%d.",_ret);
			ScreenSendCommand(WRITE_82, SERVER_INFO, (rt_uint8_t*)string,rt_strlen(string));
		}
		break;
	case KEY_SERVER_TEST:
		_ret = device_test(&send_server_data, &rev_server_data);
		rt_memset(string, 0, 30);
		ScreenSendCommand(WRITE_82, SERVER_INFO, (rt_uint8_t*)string,30);
		if(RT_EOK == _ret)
		{
			rt_sprintf(string, "���������Գɹ���");
			ScreenSendCommand(WRITE_82, SERVER_INFO, (rt_uint8_t*)string,rt_strlen(string));
		}
		else
		{
			rt_sprintf(string, "����������ʧ�ܣ�������룺%d.",_ret);
			ScreenSendCommand(WRITE_82, SERVER_INFO, (rt_uint8_t*)string,rt_strlen(string));
		}
		break;
	case KEY_SERVER_UPLOAD:	
		_ret = device_sample(&send_server_data, &server_sample);
		rt_memset(string, 0, 30);
		ScreenSendCommand(WRITE_82, SERVER_INFO, (rt_uint8_t*)string,30);
		if(RT_EOK == _ret)
		{
			rt_sprintf(string, "�ϴ����ݷ������ɹ���");
			ScreenSendCommand(WRITE_82, SERVER_INFO, (rt_uint8_t*)string,rt_strlen(string));
		}
		else
		{
			rt_sprintf(string, "�ϴ�����ʧ�ܣ�������룺%d.",_ret);
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
 * ʮ������תʮ����
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
 * ʮ����תʮ������
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
static void DealCmd(const rt_uint8_t* _ucData)
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
		case SAMPLE_CHANNEL:
			UartBuff.flow_para.channel = choose_channel(SAMPLE_CHANNEL + 2, _data);
			flow_para_display(UartBuff.flow_para.channel);
			break;
		case SAMPLE_PRO:

			break;
		case SAMPLE_MIX_TIME:
			UartBuff.flow_para.mix_time = _data;
			break;
		case SAMPLE_HEAT_TIME:
			UartBuff.flow_para.heat_time = _data;
			break;
		case SAMPLE_READ_TIME:
			UartBuff.flow_para.read_time = _data;
			break;
		case SAMPLE_A1_TIME:
			UartBuff.flow_para.a1_time = _data;
			break;
		case SAMPLE_A2_TIME:
			UartBuff.flow_para.a2_time = _data;
			break;
		case RESULT_SLIDER:			
			rt_kprintf("RESULT_SLIDER %d\n", _data);
			break;
		case RFID_SET:
			break;
		case TEMP_CHANNEL:
			rt_memset(string, 0, sizeof(string));
			ScreenSendCommand(WRITE_82, TEMP_INFO, (rt_uint8_t*)string,sizeof(string));
			UartBuff.heat_para.channel = choose_channel(TEMP_CHANNEL + 2, _data);
			display_heat_para(UartBuff.heat_para.channel);
			break;
		case TEMP_SET:
			UartBuff.heat_para.iSetVal = _data * 10;
			break;
		case TEMP_TIME:
			UartBuff.heat_para.CycleTime = _data;
			break;
		case TEMP_KP:		
			UartBuff.heat_para.uKP_Coe = (float)_data / 100;
			break;
		case TEMP_KI:			
			UartBuff.heat_para.uKI_Coe = (float)_data / 100;
			break;
		case TEMP_KD:			
			UartBuff.heat_para.uKD_Coe = (float)_data / 100;
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
		case BLENDER_TIME2:
		case BLENDER_TIME3:
		case BLENDER_TIME4:
			set_blender_duty((_addr-BLENDER_TIME1)/2, _data);
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
				break;
			case 2:
				UartBuff.MotorPara.h_Motor = &Motor2;
				break;
			case 3:
				UartBuff.MotorPara.h_Motor = &Motor3;
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
				rt_sprintf(string, "��ѡ��ͨ����");
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
				rt_sprintf(string, "��ѡ��ͨ����");
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
				rt_sprintf(string, "��ѡ��ͨ����");
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
				rt_sprintf(string, "��ѡ��ͨ����");
				ScreenSendCommand(WRITE_82, MOTOR_INFO, (rt_uint8_t*)string,rt_strlen(string));
			}
			break;
/**************����5130******************/
			/*
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
			break;*/
/**************����5130******************/		
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
		case CURVE_Y:
			SetVD_Central(_data);
			break;
		case CURVE_X:
			SetX_amp(_data);
			break;
		default:
			break;
        }
	}
	else if(0x81 == _ucData[0])
	{
		if(0x20 == _ucData[1])  // ��ȡ������RTC
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
			/* ���� Ƭ��RTC */
			set_time(UartBuff.RealTime.hour, UartBuff.RealTime.minute, 0);
			set_date(UartBuff.RealTime.year, UartBuff.RealTime.month, UartBuff.RealTime.day);
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
static void GetUartScreenCMD(rt_uint8_t _reData)
{	
	static uint8_t pos = 0;
	static uint8_t len = 0;
	switch (len)
    {
    	case 0:
			if(0xa5 == _reData)   //����֡ͷ
				len ++;
			else
				len = 0;
			break;
		case 1:
			if(0x5a == _reData)   //����֡ͷ
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
 * @brief  ��ʾ�ַ����ͷ��Ϳ�������
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
	sendBuf[i++] = HEAD_1;
	sendBuf[i++] = HEAD_2;				/* �^֡ */
	i++;
	sendBuf[i++] = _cmd;				/* ָ�� */

	if(WRITE_82 == _cmd)
	{
		sendBuf[i++] = (rt_uint8_t)(s_addr>>8 & 0x00ff);
		sendBuf[i++] = (rt_uint8_t)(s_addr & 0x00ff);/* ��ַ */
	}
	else
	{
		sendBuf[i++] = (rt_uint8_t)(s_addr & 0x00ff);/* ��ַ */
		sendlen -= 1; 
	}	
	rt_memcpy(&sendBuf[i++], str, len);
	sendBuf[2] = sendlen - 3;			/* ���� */
	for(i = 0;i < sendlen;i++)
	{
		uart_putchar(sendBuf[i]);
    }

	if(sendBuf)
		rt_free(sendBuf);
	return RT_EOK;
}

/*
 * ��ʾ���ݱ���
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
	sendBuf[i++] = HEAD_1;
	sendBuf[i++] = HEAD_2;		/* �^֡ */
	i++;
	sendBuf[i++] = WRITE_82;	/* ָ�� */

	sendBuf[i++] = (rt_uint8_t)(s_addr>>8 & 0x00ff);
	sendBuf[i++] = (rt_uint8_t)(s_addr & 0x00ff);/* ��ַ */
	
	while(len > 0)
	{
		sendBuf[i++] = str[len-1];
		len--;
	}
	sendBuf[2] = sendlen - 3;			/* ���� */
	for(i = 0;i < sendlen;i++)
	{
		uart_putchar(sendBuf[i]);
    }

	if(sendBuf)
		rt_free(sendBuf);
	return RT_EOK;
}
/*
 * ��ʾ���ݱ���(�����ֽ�)
 */
rt_uint8_t ScreenSendData_2bytes(rt_uint16_t s_addr, rt_uint16_t _data)
{
	rt_uint8_t sendBuf[8] = {0};
	rt_uint8_t i = 0;
		
	sendBuf[i++] = HEAD_1;
	sendBuf[i++] = HEAD_2;				/* �^֡ */
	sendBuf[i++] = 5;					/* ���� */
	sendBuf[i++] = WRITE_82;			/* ָ�� */

	sendBuf[i++] = (rt_uint8_t)(s_addr>>8 & 0x00ff);
	sendBuf[i++] = (rt_uint8_t)(s_addr & 0x00ff);		/* ��ַ */

	sendBuf[i++] = (rt_uint8_t)(_data>>8 & 0x00ff);
	sendBuf[i++] = (rt_uint8_t)(_data & 0x00ff);		/* ���� */	
	
	for(i = 0;i < sizeof(sendBuf);i++)
	{
		uart_putchar(sendBuf[i]);
    }
	
	return RT_EOK;
}
/*
 * ��ʾͼ�����
 */
rt_uint8_t ScreenDisICON(rt_uint16_t s_addr, rt_uint8_t status)
{
	/* 								�^֡       ����    ָ��            ��ַ        ����  */
	rt_uint8_t sendBuf[8] = {HEAD_1, HEAD_2, 5, WRITE_82, 0, 0, 0, status};

	sendBuf[4] = (rt_uint8_t)(s_addr>>8 & 0x00ff);
	sendBuf[5] = (rt_uint8_t)(s_addr & 0x00ff);	/* ��ַ */

	for(rt_uint8_t i = 0;i < 8;i++)
	{
		uart_putchar(sendBuf[i]);
    }
	
	return RT_EOK;
}
/**
 * @brief  д������
 *
 * @param 	_ch:ͨ��
 * 			_ch��ÿһ��bit��Ӧһ��ͨ����0x0f-->ʹ��1~4ͨ��
 *			_data������
 * @return
 */
rt_uint8_t ScreenCurve(rt_uint8_t _ch, rt_uint16_t* _data)
{
	rt_uint8_t* sendBuf = RT_NULL;
	rt_uint8_t n = 0, i = 0, len = 0;

	for(i = 0;i < 4;i++)
	{
		if(_ch & (0x01<<i))
			len++;
	}
	if(0 == len)
		return RT_ERROR;
	sendBuf = rt_malloc(len*2 + 5);
	if(RT_NULL == sendBuf)
		return RT_ERROR;
	sendBuf[n++] = 0xa5;
	sendBuf[n++] = 0x5a;				/* �^֡ */
	sendBuf[n++] = 2 + len*2;			/* ���� */
	sendBuf[n++] = CURVE_84;			/* ָ�� */

	sendBuf[n++] = _ch;		/* ͨ�� */
	for(i = 0;i < len;i++)
	{
		sendBuf[n++] = (rt_uint8_t)(_data[i]>>8 & 0x00ff);
		sendBuf[n++] = (rt_uint8_t)(_data[i] & 0x00ff);		/* ���� */
	}

	for(i = 0;i < n;i++)
	{
		uart_putchar(sendBuf[i]);
    }
	if(sendBuf)
		rt_free(sendBuf);
	return RT_EOK;
}
/*
 * ������������
 */
rt_uint8_t CurveClear(void)
{
	rt_uint8_t d = 0x55;
	return ScreenSendCommand(WRITE_80, 0xeb, &d, 1);
}
/*
 * ��������Y������
 */
rt_uint8_t SetVD_Central(rt_uint16_t range)
{
	rt_uint8_t _range[2] = {(range/2) & 0x00ff, (range/2)>>8 & 0x00ff};
	rt_uint8_t _amp[2] = {0, 0};
	rt_uint16_t amp_value = 430 * 256 / range;
	_amp[1] = amp_value>>8 & 0x00ff;
	_amp[0] = amp_value & 0x00ff;

	for(rt_uint8_t i = 0; i<4; i++)
	{
		ScreenSendData(CURVE1_DESC | 0x06 | (0x20*i), _range, 2); /* ����Y������ */
		rt_thread_delay(10);
		ScreenSendData(CURVE1_DESC | 0x08 | (0x20*i), _amp, 2); /* ����Y��Ŵ��� */
		rt_thread_delay(10);
	}

	return 0;
}
/*
 * ��������x��Ŵ���
 */
rt_uint8_t SetX_amp(rt_uint8_t _amp)
{
	rt_uint8_t temp[2] = {_amp, 0};
	for(rt_uint8_t i = 0; i<4; i++)
	{
		temp[1] = i;
		ScreenSendData(CURVE1_DESC | 0x09 | (0x20*i), temp, 2);
		rt_thread_delay(5);
	}
	return 0;
}
rt_err_t uart_open(const char *name)
{
    rt_err_t res;

    /* ����ϵͳ�еĴ����豸 */
    uart_device = rt_device_find(name);   
    /* ���ҵ��豸����� */
    if (uart_device != RT_NULL)
    {          
        res = rt_device_set_rx_indicate(uart_device, uart_intput);
        /* ��鷵��ֵ */
        if (res != RT_EOK)
        {
            rt_kprintf("set %s rx indicate error.%d\n",name,res);
            return -RT_ERROR;
        }

        /* ���豸���Կɶ�д���жϷ�ʽ */
        res = rt_device_open(uart_device, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX );       
        /* ��鷵��ֵ */
        if (res != RT_EOK)
        {
            rt_kprintf("open %s device error.%d\n",name,res);
            return -RT_ERROR;
        }

        /* ��ʼ���¼����� */
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
		rt_thread_delay(50);
	}
}

