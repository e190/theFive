/*
 * File      : drv_i2c.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006 - 2018, RT-Thread Development Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-06-05     tanek        first implementation.
 * 2018-03-08     ZYH          Porting for stm32f4xx
 *
 * 2018-08-21     sniper       ���Ӷ��I2C
 */
#include "drv_i2c.h"

#if defined(RT_USING_I2C_1)
/*user can change this*/
#define I2C_1_NAME  "i2c1"
struct drv_i2c i2c_bus_1; //ad1110
#endif /* RT_USING_I2C_1 */

#if defined(RT_USING_I2C_2)
#define I2C_2_NAME  "i2c2"
struct drv_i2c i2c_bus_2; //ad1110
#endif /* RT_USING_I2C_2 */

#if defined(RT_USING_I2C_3)
/*user can change this*/
#define I2C_3_NAME  "i2c3"
struct drv_i2c i2c_bus_3; //ad1110
#endif /* RT_USING_I2C_3 */

#if defined(RT_USING_I2C_4)
#define I2C_4_NAME  "i2c4"
struct drv_i2c i2c_bus_4; //ad1110
#endif /* RT_USING_I2C_4 */

#if defined(RT_USING_I2C_5)
#define I2C_5_NAME  "i2c5"
struct drv_i2c i2c_bus_5; //rfid
#endif /* RT_USING_I2C_5 */

static void drv_i2c_gpio_init()
{	
    GPIO_InitTypeDef GPIO_Initure;
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
	
#if defined(RT_USING_I2C_1)	
	i2c_bus_1.scl_pin = GPIO_PIN_12;
	i2c_bus_1.scl_port = GPIOB;
	i2c_bus_1.sda_pin = GPIO_PIN_6;
	i2c_bus_1.sda_port = GPIOC;

    GPIO_Initure.Pin = GPIO_PIN_12;
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_Initure.Pull = GPIO_NOPULL;
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_Initure);

    GPIO_Initure.Pin = GPIO_PIN_6;
    HAL_GPIO_Init(GPIOC, &GPIO_Initure);

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
#endif /* RT_USING_I2C_1 */
	
#if defined(RT_USING_I2C_2)
	i2c_bus_2.scl_pin = GPIO_PIN_6;
	i2c_bus_2.scl_port = GPIOB;
	i2c_bus_2.sda_pin = GPIO_PIN_7;
	i2c_bus_2.sda_port = GPIOB;

    GPIO_Initure.Pin = GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_Initure.Pull = GPIO_NOPULL;
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_Initure);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6 | GPIO_PIN_7, GPIO_PIN_SET);
#endif /* RT_USING_I2C_2 */
	
#if defined(RT_USING_I2C_3)
	i2c_bus_3.scl_pin = GPIO_PIN_1;
	i2c_bus_3.scl_port = GPIOF;
	i2c_bus_3.sda_pin = GPIO_PIN_0;
	i2c_bus_3.sda_port = GPIOF;

    GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_1;
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_Initure.Pull = GPIO_NOPULL;
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOF, &GPIO_Initure);
    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_0 | GPIO_PIN_1, GPIO_PIN_SET);
#endif /* RT_USING_I2C_3 */

#if defined(RT_USING_I2C_4)
	i2c_bus_4.scl_pin = GPIO_PIN_8;
	i2c_bus_4.scl_port = GPIOG;
	i2c_bus_4.sda_pin = GPIO_PIN_7;
	i2c_bus_4.sda_port = GPIOC;

    GPIO_Initure.Pin = GPIO_PIN_7;
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_Initure.Pull = GPIO_NOPULL;
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOC, &GPIO_Initure);

    GPIO_Initure.Pin = GPIO_PIN_8;
    HAL_GPIO_Init(GPIOG, &GPIO_Initure);

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_8, GPIO_PIN_SET);
#endif /* RT_USING_I2C_4 */

#if defined(RT_USING_I2C_5)	
	i2c_bus_5.scl_pin = GPIO_PIN_0;
	i2c_bus_5.scl_port = GPIOD;
	i2c_bus_5.sda_pin = GPIO_PIN_1;
	i2c_bus_5.sda_port = GPIOD;
	
    GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_1;
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_Initure.Pull = GPIO_NOPULL;
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOD, &GPIO_Initure);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0 | GPIO_PIN_1, GPIO_PIN_SET);
#endif /* RT_USING_I2C_5 */	
	
}

static void drv_set_sda(struct rt_i2c_bus_device *bus, void *data, rt_int32_t state)
{
	struct drv_i2c* i2c_drv;
	i2c_drv = (struct drv_i2c*)bus->pin;
    HAL_GPIO_WritePin(i2c_drv->sda_port, i2c_drv->sda_pin, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

static void drv_set_scl(struct rt_i2c_bus_device *bus, void *data, rt_int32_t state)
{
	struct drv_i2c* i2c_drv;
	i2c_drv = (struct drv_i2c*)bus->pin;
    HAL_GPIO_WritePin(i2c_drv->scl_port, i2c_drv->scl_pin, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

static rt_int32_t  drv_get_sda(struct rt_i2c_bus_device *bus, void *data)
{
	struct drv_i2c* i2c_drv;
	i2c_drv = (struct drv_i2c*)bus->pin;
	return HAL_GPIO_ReadPin(i2c_drv->sda_port, i2c_drv->sda_pin) ? 1 : 0;
}

static rt_int32_t  drv_get_scl(struct rt_i2c_bus_device *bus, void *data)
{
	struct drv_i2c* i2c_drv;
	i2c_drv = (struct drv_i2c*)bus->pin;
	return HAL_GPIO_ReadPin(i2c_drv->scl_port, i2c_drv->scl_pin) ? 1 : 0;
}

static void drv_udelay(rt_uint32_t us)
{
    int i = (HAL_RCC_GetHCLKFreq() / 4000000 * us);
    while (i)
    {
        i--;
    }
}

static const struct rt_i2c_bit_ops drv_bit_ops =
{
    RT_NULL,
    drv_set_sda,
    drv_set_scl,
    drv_get_sda,
    drv_get_scl,
    drv_udelay,
    1,
    100
};

int drv_i2c_init(void)
{
    static struct rt_i2c_bus_device i2c_bus1;
	static struct rt_i2c_bus_device i2c_bus2;
    static struct rt_i2c_bus_device i2c_bus3;
	static struct rt_i2c_bus_device i2c_bus4;
	static struct rt_i2c_bus_device i2c_bus5;
    drv_i2c_gpio_init();

#if defined(RT_USING_I2C_1)	
    rt_memset((void *)&i2c_bus1, 0, sizeof(struct rt_i2c_bus_device));
	i2c_bus1.pin = &i2c_bus_1;
    i2c_bus1.priv = (void *)&drv_bit_ops;
    rt_i2c_bit_add_bus(&i2c_bus1, I2C_1_NAME);
#endif /* RT_USING_I2C_1 */

#if defined(RT_USING_I2C_2)
    rt_memset((void *)&i2c_bus2, 0, sizeof(struct rt_i2c_bus_device));
	i2c_bus2.pin = &i2c_bus_2;
    i2c_bus2.priv = (void *)&drv_bit_ops;
    rt_i2c_bit_add_bus(&i2c_bus2, I2C_2_NAME);
#endif /* RT_USING_I2C_2 */

#if defined(RT_USING_I2C_3)
    rt_memset((void *)&i2c_bus3, 0, sizeof(struct rt_i2c_bus_device));
	i2c_bus3.pin = &i2c_bus_3;
    i2c_bus3.priv = (void *)&drv_bit_ops;
    rt_i2c_bit_add_bus(&i2c_bus3, I2C_3_NAME);
#endif /* RT_USING_I2C_3 */

#if defined(RT_USING_I2C_4)
    rt_memset((void *)&i2c_bus4, 0, sizeof(struct rt_i2c_bus_device));
	i2c_bus4.pin = &i2c_bus_4;
    i2c_bus4.priv = (void *)&drv_bit_ops;
    rt_i2c_bit_add_bus(&i2c_bus4, I2C_4_NAME);
#endif /* RT_USING_I2C_4 */

#if defined(RT_USING_I2C_5)	
    rt_memset((void *)&i2c_bus5, 0, sizeof(struct rt_i2c_bus_device));
	i2c_bus5.pin = &i2c_bus_5;
    i2c_bus5.priv = (void *)&drv_bit_ops;
    rt_i2c_bit_add_bus(&i2c_bus5, I2C_5_NAME);
#endif /* RT_USING_I2C_5 */
	
    return RT_EOK;
}
INIT_DEVICE_EXPORT(drv_i2c_init);
/* end of i2c driver */
