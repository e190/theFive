#include <board.h>
#include <dfs_fs.h>
#include "RunLED.h"
#include "bsp_G780.h"
#include "ServerData.h"
#include "bsp_rfid.h"
#include "Uart_Screen.h"
//#include "bsp_tmc5130.h"
//#include "TMC5130.h"
#include "DataBase.h"

rt_device_t dhidd;

void GpioDeviceInit(void)
{                
	rt_pin_mode(RunLED_gpio,PIN_MODE_OUTPUT);     //设置Pin为输出模式。

	/* 直射LED */
	rt_pin_mode(LED1_gpio,PIN_MODE_OUTPUT);
	rt_pin_mode(LED2_gpio,PIN_MODE_OUTPUT);
	rt_pin_mode(LED3_gpio,PIN_MODE_OUTPUT);
	rt_pin_mode(LED4_gpio,PIN_MODE_OUTPUT);
	rt_pin_mode(LED5_gpio,PIN_MODE_OUTPUT);
	rt_pin_mode(LED6_gpio,PIN_MODE_OUTPUT);
	rt_pin_mode(LED7_gpio,PIN_MODE_OUTPUT);
	rt_pin_mode(LED8_gpio,PIN_MODE_OUTPUT);

	rt_pin_mode(SD_IN_gpio, PIN_MODE_INPUT_PULLUP);
	rt_pin_mode(15,PIN_MODE_OUTPUT);   //  PF5
	rt_pin_write(15, !rt_pin_read(15));
}


void Function_RunLED(void* parameter)
{
	rt_uint8_t aa = 0;
	rt_uint8_t addr = 5,bb = 0;
	char data[64] = "this is hid test \n";
	GpioDeviceInit();

	while(1)
	{
		rt_pin_write(RunLED_gpio, !rt_pin_read(RunLED_gpio)); //GPIO输出状态为：当前状态的反向状态。
//		if(0 == rt_pin_read(SD_IN_gpio))
//			rt_kprintf("sd coming\n");

		if(1 == aa)
		{
			aa = 0;
//			bb++;
//			flash_test_demo(bb);
//			if(bb > 16)
//			{
//				aa = 0;
//				rt_kprintf("------------------------\n");
//			}
			rt_device_write(dhidd, addr, data, 65);
		}	
		else if(2 == aa)
		{
			aa = 0;			
			dhidd = rt_device_find("hidd");
			if(dhidd != RT_NULL)
				rt_device_open(dhidd, RT_DEVICE_FLAG_RDWR);
			//flash_read_demo(bb);
		}
		else if(3 == aa)
		{
			aa = 0;
			//load_flash();
			if(dfs_mount("sd0", "/", "elm", 0, 0) == 0)
			{
				rt_kprintf("sd card mount to / success!\n");
			}
			else
			{
				rt_kprintf("sd card mount to / failed!\n");
			}
		}

		rt_thread_delay(500);       //等待500ms
	}
}
