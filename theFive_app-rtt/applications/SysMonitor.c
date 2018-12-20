#include <rtthread.h>
#include <board.h>
#include "SysMonitor.h"
#include "bsp_rfid.h"
#include "Uart_Screen.h"
#include "usb_hid.h"
#include "DataBase.h"


void Function_sys_monitor(void* parameter)
{
	rt_uint8_t temp_count = 0;
	rt_uint8_t aa = 0;
	rt_uint8_t addr = 5,bb = 0;

	while(1)
	{
		rt_pin_write(RUN_LED_PIN, !rt_pin_read(RUN_LED_PIN)); //GPIO输出状态为：当前状态的反向状态。
		hid_write_timeout();
		cycle_temp_heat();
		dis_temp_status();

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
			load_flash();
			//flash_read_demo(bb);
		}
		else if(3 == aa)
		{
			aa = 0;
			//TestRFID();
		}

		rt_thread_delay(500);       //等待500ms
	}
}
