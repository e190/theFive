#include <rthw.h>
#include <rtthread.h>
#include "bsp_ds18b20.h"

OneWire_t OneWire1;
	

static void bsp_udelay(rt_uint32_t us)
{
    int i = (HAL_RCC_GetHCLKFreq() / 4000000 * us);
    while (i)
    {
        i--;
    }
}

rt_err_t ds18b20_reset(OneWire_t* OneWire)	   
{           
	uint8_t i;
	uint16_t k;
	register rt_base_t level;

	/* disable interrupt */
    level = rt_hw_interrupt_disable();
	//rt_enter_critical();

	/* ��λ�����ʧ���򷵻�0 */
	for (i = 0; i < 1; i++)
	{
		ONEWIRE_LOW(OneWire);				/* ����DQ */
		bsp_udelay(520);	/* �ӳ� 520uS�� Ҫ������ӳٴ��� 480us */
		ONEWIRE_HIGH(OneWire);				/* �ͷ�DQ */

		bsp_udelay(15);	/* �ȴ�15us */

		/* ���DQ��ƽ�Ƿ�Ϊ�� */
		for (k = 0; k < 10; k++)
		{
			if (0 == GPIO_GetInputPinValue(OneWire->GPIOx, OneWire->GPIO_Pin))
			{
				//rt_kprintf("reset\r\n");
				break;
			}
			bsp_udelay(10);	/* �ȴ�65us */
		}
		if (k >= 10)
		{
			continue;		/* ʧ�� */
		}

		/* �ȴ�DS18B20�ͷ�DQ */
		for (k = 0; k < 30; k++)
		{
			if (GPIO_GetInputPinValue(OneWire->GPIOx, OneWire->GPIO_Pin))
			{
				break;
			}
			bsp_udelay(10);	/* �ȴ�65us */
		}
		if (k >= 30)
		{
			continue;		/* ʧ�� */
		}

		break;
	}

	rt_hw_interrupt_enable(level);/* ʹ��ȫ���ж� */

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

	/* ���߸�λ */
	if (ds18b20_reset(OneWire) == 0)
	{
		return 0;
	}		

	ds18b20_write_byte(OneWire, 0xcc);	/* ������ */
	ds18b20_write_byte(OneWire, 0x44);	/* ��ת������ */

	ds18b20_reset(OneWire);		/* ���߸�λ */

	ds18b20_write_byte(OneWire, 0xcc);	/* ������ */
	ds18b20_write_byte(OneWire, 0xbe);

	temp1 = ds18b20_read_byte(OneWire);	/* ���¶�ֵ���ֽ� */
	temp2 = ds18b20_read_byte(OneWire);	/* ���¶�ֵ���ֽ� */

	return ((temp2 << 8) | temp1);	/* ����16λ�Ĵ���ֵ */
}
rt_uint8_t ds18b20_read_ID(OneWire_t* OneWire)
{
	rt_uint8_t _ret= 0;
	/* ���߸�λ */
	if (ds18b20_reset(OneWire) == 0)
	{
		return 0;
	}

	ds18b20_write_byte(OneWire, 0x33);	/* ������ */
	
	_ret = ds18b20_read_byte(OneWire);

	ds18b20_reset(OneWire);		/* ���߸�λ */
	
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
	
	return RT_EOK;
}
INIT_DEVICE_EXPORT(ds18b20_hw_init);
