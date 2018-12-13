#include <rtthread.h>
#include <drivers/pin.h>
#include <board.h>
#include <dfs_fs.h>
#include "SysMonitor.h"
#include "bsp_rfid.h"
#include "Uart_Screen.h"
#include "usb_hid.h"
//#include "bsp_tmc5130.h"
//#include "TMC5130.h"
#include "DataBase.h"
#include "dc_motor.h"


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

//	rt_pin_mode(SD_IN_gpio, PIN_MODE_INPUT_PULLUP);
//	rt_pin_mode(15,PIN_MODE_OUTPUT);   //  PF5
//	rt_pin_write(15, !rt_pin_read(15));
//	rt_pin_mode(13,PIN_MODE_OUTPUT);   //  PF3
//	rt_pin_write(13, !rt_pin_read(15));
}


void Function_sys_monitor(void* parameter)
{
	rt_uint8_t temp_count = 0;
	rt_uint8_t aa = 0;
	rt_uint8_t addr = 5,bb = 0;

	GpioDeviceInit();

	while(1)
	{
		rt_pin_write(RunLED_gpio, !rt_pin_read(RunLED_gpio)); //GPIO输出状态为：当前状态的反向状态。
		//hid_write_timeout();
		cycle_temp_heat();
		dis_temp_status();

//		if(0 == rt_pin_read(SD_IN_gpio))
//			rt_kprintf("sd coming\n");
//		if(0 == rt_pin_read(118))
//			rt_kprintf("RFID coming\n");
		if(1 == aa)
		{

			RT_ASSERT(0);
			aa = 0;
//			bb++;
//			flash_test_demo(bb);
//			if(bb > 16)
//			{
//				aa = 0;
//				rt_kprintf("------------------------\n");
//			}
		}	
		else if(2 == aa)
		{
			aa = 0;			
			//flash_read_demo(bb);
		}
		else if(3 == aa)
		{
			aa = 0;
			//TestRFID();
			//load_flash();
//			if(dfs_mount("sd0", "/", "elm", 0, 0) == 0)
//			{
//				rt_kprintf("sd card mount to / success!\n");
//			}
//			else
//			{
//				rt_kprintf("sd card mount to / failed!\n");
//			}
		}

		rt_thread_delay(500);       //等待500ms
	}
}
