#include <rthw.h>
#include <rtthread.h>
#include "bsp_ds18b20.h"

OneWire_t OneWire1, OneWire2, OneWire3, OneWire4;

/**
 *	很准 us
 */
static void bsp_udelay(rt_uint32_t us)
{
    int i = (HAL_RCC_GetHCLKFreq() / 2000000 * us / 5);
    while (i)
    {
        i--;
    }
}

rt_err_t ds18b20_reset(OneWire_t* OneWire)	   
{           
	uint8_t i;
	uint16_t k;

	/*
		复位时序, 见DS18B20 page 15

		首先主机拉低DQ，持续最少 480us
		然后释放DQ，等待DQ被上拉电阻拉高，约 15-60us
		DS18B20 将驱动DQ为低 60-240us， 这个信号叫 presence pulse  (在位脉冲,表示DS18B20准备就绪 可以接受命令)
		如果主机检测到这个低应答信号，表示DS18B20复位成功
	*/

	/* 复位，如果失败则返回0 */
	for (i = 0; i < 1; i++)
	{
		ONEWIRE_LOW(OneWire);				/* 拉低DQ */
		bsp_udelay(520);	/* 延迟 520uS， 要求这个延迟大于 480us */
		ONEWIRE_HIGH(OneWire);				/* 释放DQ */

		bsp_udelay(15);	/* 等待15us */
		/* 检测DQ电平是否为低 */
		for (k = 0; k < 10; k++)
		{
			if (0 == GPIO_GetInputPinValue(OneWire->GPIOx, OneWire->GPIO_Pin))
			{
				//rt_kprintf("reset\r\n");
				break;
			}
			bsp_udelay(10);	/* 等待65us */
		}
		if (k >= 10)
		{
			continue;		/* 失败 */
		}

		/* 等待DS18B20释放DQ */
		for (k = 0; k < 30; k++)
		{
			if (GPIO_GetInputPinValue(OneWire->GPIOx, OneWire->GPIO_Pin))
			{
				break;
			}
			bsp_udelay(10);	/* 等待65us */
		}
		if (k >= 30)
		{
			continue;		/* 失败 */
		}

		break;
	}

	bsp_udelay(5);

	if (i >= 1)
	{
		return 0;
	}

	return 1;
}

rt_err_t ds18b20_write_byte(OneWire_t* OneWire, rt_uint8_t data)
{
	uint8_t i;

	for (i = 0; i < 8; i++)
	{
		ONEWIRE_LOW(OneWire);
		bsp_udelay(2);

		if (data & 0x01)	
			ONEWIRE_HIGH(OneWire);		
		else		
			ONEWIRE_LOW(OneWire);
		
		bsp_udelay(60);
		ONEWIRE_HIGH(OneWire);
		bsp_udelay(2);
		data >>= 1;
	}
	return RT_EOK;
}
rt_uint8_t ds18b20_read_byte(OneWire_t* OneWire)
{
	rt_uint8_t i;
	rt_uint8_t read = 0;

	for (i = 0; i < 8; i++)
	{
		read >>= 1;

		ONEWIRE_LOW(OneWire);
		bsp_udelay(3);
		ONEWIRE_HIGH(OneWire);
		bsp_udelay(3);

		if (GPIO_GetInputPinValue(OneWire->GPIOx, OneWire->GPIO_Pin))		
			read |= 0x80;
		
		bsp_udelay(60);
	}
	return read;
}
rt_uint16_t ds18b20_read_reg(OneWire_t* OneWire)
{
	rt_uint8_t temp1, temp2;

	/* 总线复位 */
	if (ds18b20_reset(OneWire) == 0)
	{
		return 0;
	}		

	ds18b20_write_byte(OneWire, 0xcc);	/* 发命令 */
	ds18b20_write_byte(OneWire, 0x44);	/* 发转换命令 */
	ds18b20_reset(OneWire);		/* 总线复位 */
	ds18b20_write_byte(OneWire, 0xcc);	/* 发命令 */
	ds18b20_write_byte(OneWire, 0xbe);

	temp1 = ds18b20_read_byte(OneWire);	/* 读温度值低字节 */
	temp2 = ds18b20_read_byte(OneWire);	/* 读温度值高字节 */

	return ((temp2 << 8) | temp1);	/* 返回16位寄存器值 */
}
rt_uint8_t ds18b20_read_ID(OneWire_t* OneWire)
{
	rt_uint8_t _ret= 0;
	/* 总线复位 */
	if (ds18b20_reset(OneWire) == 0)
	{
		return 0;
	}

	ds18b20_write_byte(OneWire, 0x33);	/* 发命令 */
	
	_ret = ds18b20_read_byte(OneWire);

	ds18b20_reset(OneWire);		/* 总线复位 */
	
	return _ret;
}
static void ds18b20_gpio_init(OneWire_t* OneWireStruct, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOG_CLK_ENABLE();
	
    GPIO_Initure.Pin = GPIO_Pin;
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_Initure.Pull = GPIO_NOPULL;
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOx, &GPIO_Initure);

    HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_SET);
	
	/* Save settings */
	OneWireStruct->GPIOx = GPIOx;
	OneWireStruct->GPIO_Pin = GPIO_Pin;
}

int ds18b20_hw_init(void)
{
	ds18b20_gpio_init(&OneWire1, GPIOG, GPIO_PIN_9);
	ds18b20_gpio_init(&OneWire2, GPIOG, GPIO_PIN_7);
	ds18b20_gpio_init(&OneWire3, GPIOG, GPIO_PIN_6);
	ds18b20_gpio_init(&OneWire4, GPIOG, GPIO_PIN_5);
	
	return RT_EOK;
}
INIT_DEVICE_EXPORT(ds18b20_hw_init);
