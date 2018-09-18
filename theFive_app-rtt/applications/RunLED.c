#include "RunLED.h"
#include "bsp_G780.h"
#include "ServerData.h"
#include "bsp_rfid.h"
#include "bsp_tmc5130.h"
#include "TMC5130.h"

rt_device_t dhidd;
rt_uint8_t data[64] = {5,9,8,7,6,5,1,23,4};
void GpioDeviceInit(void)
{                
	rt_pin_mode(RunLED_gpio,PIN_MODE_OUTPUT);     //设置Pin为输出模式。

	/* 直射LED */
	rt_pin_mode(LED1_gpio,PIN_MODE_OUTPUT);  //定义Pin：GPIO_G_13。  	
	rt_pin_mode(LED2_gpio,PIN_MODE_OUTPUT);  //定义Pin：GPIO_G_13。  
	//rt_pin_mode(LED3_gpio,PIN_MODE_OUTPUT);  //定义Pin：GPIO_G_13。
	//rt_pin_mode(LED4_gpio,PIN_MODE_OUTPUT);  //定义Pin：GPIO_G_13。
	
	/* 开关门 */
	rt_pin_mode(Ldoor_en_gpio,PIN_MODE_OUTPUT);  //定义Pin：GPIO_B_10。  	
	rt_pin_mode(Rdoor_en_gpio,PIN_MODE_OUTPUT);  //定义Pin：GPIO_E_14。 
	rt_pin_mode(Ldoormotor1_gpio,PIN_MODE_OUTPUT);  //定义Pin：GPIO_B_10。  	
	rt_pin_mode(Ldoormotor2_gpio,PIN_MODE_OUTPUT);  //定义Pin：GPIO_E_14。  
	rt_pin_mode(Rdoormotor1_gpio,PIN_MODE_OUTPUT);  //定义Pin：GPIO_F_7。
	rt_pin_mode(Rdoormotor2_gpio,PIN_MODE_OUTPUT);  //定义Pin：GPIO_F_8。
	rt_pin_write(Ldoor_en_gpio, 0);
	rt_pin_write(Rdoor_en_gpio, 0);
	/* 搅拌电机 */	
	//rt_pin_mode(blender1_gpio,PIN_MODE_OUTPUT);  //定义Pin：GPIO_G_13。  	
	//rt_pin_mode(blender2_gpio,PIN_MODE_OUTPUT);  //定义Pin：GPIO_G_13。  
	//rt_pin_mode(blender3_gpio,PIN_MODE_OUTPUT);  //定义Pin：GPIO_G_13。
	rt_pin_mode(blender4_gpio, PIN_MODE_OUTPUT);  //定义Pin：GPIO_B_0。
	
	rt_pin_write(blender4_gpio, 1);
	 
	rt_pin_mode(91,PIN_MODE_OUTPUT);
	rt_pin_write(91, !(_Bool)rt_pin_read(91));
}


void Function_RunLED(void* parameter)
{
	rt_uint8_t aa = 0;
	uint32_t addr = 5,bb = 0;

	GpioDeviceInit();

	while(1)
	{
		rt_pin_write(RunLED_gpio, !(_Bool)rt_pin_read(RunLED_gpio)); //GPIO输出状态为：当前状态的反向状态。
		if(1 == aa)
		{
			aa = 0;
			
			rt_device_write(dhidd, addr, data, 65);	
			bb = tmc5130_readInt(TMC5130_XACTUAL);
			rt_kprintf("%x\n",bb);
		}	
		else if(2 == aa)
		{
			aa = 0;			
			//TestRFID();
			dhidd = rt_device_find("hidd");
			if(dhidd != RT_NULL)
				rt_device_open(dhidd, RT_DEVICE_FLAG_RDWR);
		}
		else if(3 == aa)
		{
			aa = 0;

		}
		//rt_kprintf("aff\n");
		rt_thread_delay(500);       //等待500
	}
}

