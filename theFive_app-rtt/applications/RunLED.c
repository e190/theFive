#include <board.h>
#include "RunLED.h"
#include "bsp_G780.h"
#include "ServerData.h"
#include "bsp_rfid.h"
//#include "bsp_tmc5130.h"
//#include "TMC5130.h"
#include "DataBase.h"

rt_device_t dhidd;
//rt_uint8_t data[64] = {5,9,8,7,6,5,1,23,4};
void GpioDeviceInit(void)
{                
	rt_pin_mode(RunLED_gpio,PIN_MODE_OUTPUT);     //����PinΪ���ģʽ��

	/* ֱ��LED */
	rt_pin_mode(LED1_gpio,PIN_MODE_OUTPUT);
	rt_pin_mode(LED2_gpio,PIN_MODE_OUTPUT);
	rt_pin_mode(LED3_gpio,PIN_MODE_OUTPUT);
	rt_pin_mode(LED4_gpio,PIN_MODE_OUTPUT);
	rt_pin_mode(LED5_gpio,PIN_MODE_OUTPUT);
	rt_pin_mode(LED6_gpio,PIN_MODE_OUTPUT);
	rt_pin_mode(LED7_gpio,PIN_MODE_OUTPUT);
	rt_pin_mode(LED8_gpio,PIN_MODE_OUTPUT);
	
	/* ������ */
	rt_pin_mode(Ldoor_en_gpio,PIN_MODE_OUTPUT);  //����Pin��GPIO_B_10��  	
	rt_pin_mode(Rdoor_en_gpio,PIN_MODE_OUTPUT);  //����Pin��GPIO_E_14�� 
	rt_pin_mode(Ldoormotor1_gpio,PIN_MODE_OUTPUT);  //����Pin��GPIO_B_10��  	
	rt_pin_mode(Ldoormotor2_gpio,PIN_MODE_OUTPUT);  //����Pin��GPIO_E_14��  
	rt_pin_mode(Rdoormotor1_gpio,PIN_MODE_OUTPUT);  //����Pin��GPIO_F_7��
	rt_pin_mode(Rdoormotor2_gpio,PIN_MODE_OUTPUT);  //����Pin��GPIO_F_8��
	rt_pin_write(Ldoor_en_gpio, 0);
	rt_pin_write(Rdoor_en_gpio, 0);

	rt_pin_mode(SD_IN_gpio, PIN_MODE_INPUT_PULLUP);
//	rt_pin_mode(91,PIN_MODE_OUTPUT);
//	rt_pin_write(91, !(_Bool)rt_pin_read(91));
}


void Function_RunLED(void* parameter)
{
	rt_uint8_t aa = 0;
	rt_uint8_t addr = 5,bb = 0;

	GpioDeviceInit();

	while(1)
	{
		rt_pin_write(RunLED_gpio, !rt_pin_read(RunLED_gpio)); //GPIO���״̬Ϊ����ǰ״̬�ķ���״̬��
//		if(0 == rt_pin_read(SD_IN_gpio))
//			rt_kprintf("sd coming\n");
		if(1 == aa)
		{
			//aa = 0;
			bb++;
			flash_test_demo(bb);
			if(bb > 16)
			{
				aa = 0;
				rt_kprintf("------------------------\n");
			}

//			rt_device_write(dhidd, addr, data, 65);
//			rt_kprintf("%x\n",bb);
		}	
		else if(2 == aa)
		{
			aa = 0;			
//			dhidd = rt_device_find("hidd");
//			if(dhidd != RT_NULL)
//				rt_device_open(dhidd, RT_DEVICE_FLAG_RDWR);
			flash_read_demo(bb);
		}
		else if(3 == aa)
		{
			aa = 0;
			load_flash();
		}
		rt_thread_delay(500);       //�ȴ�500
	}
}
