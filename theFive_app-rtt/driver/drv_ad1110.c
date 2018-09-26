#include "drv_ad1110.h"

#define METHOD 1

static struct rt_i2c_bus_device *ad1110_i2c_bus_1;    /* I2C�豸��� */
static struct rt_i2c_bus_device *ad1110_i2c_bus_2;

#if METHOD == 0
/**
 * @brief  дAD1110�����Ĵ���
 *  
 * @param _addr��������ַ
 * 		   reg:�Ĵ�����ַ
 *		  data:����
 * @return ����ֵ: 0,���� / -1,�������
 */
rt_err_t AD1110_write_reg(rt_uint8_t reg, rt_uint8_t data)
{
    struct rt_i2c_msg msgs;
    rt_uint8_t buf[2] = {reg, data};

    msgs.addr  = AD1110_ADDR;     /* �ӻ���ַ */
    msgs.flags = RT_I2C_WR;       /* д��־ */
    msgs.buf   = buf;             /* ��������ָ�� */
    msgs.len   = 2;

    if (rt_i2c_transfer(ad1110_i2c_bus, &msgs, 1) == 1)
    {
        return RT_EOK;
    }
    else
    {
        return -RT_ERROR;
    }
}
#elif METHOD == 1
rt_err_t ad1110_write_reg(rt_uint8_t _addr, rt_uint8_t reg, rt_uint8_t data)
{
    rt_uint8_t buf[2];

    buf[0] = reg;
    buf[1] = data;

    if (rt_i2c_master_send(ad1110_i2c_bus_1, _addr, 0, buf, 2) == 2)
    {
        return RT_EOK;
    }
    else
    {
        return -RT_ERROR;
    }
}
#endif
/**
 * @brief  ��ȡ�Ĵ�������
 *  
 * @param _ch:��ȡͨ��(1~4)
 *		_addr:������ַ
 *		  buf:��ȡ�������ݴ洢��
 * @return ����ֵ: 0,���� / -1,�������
 */
rt_err_t ad1110_read_reg(rt_uint8_t _ch, rt_uint8_t _addr, rt_uint8_t *buf)
{
    struct rt_i2c_msg msgs[2];
    struct rt_i2c_bus_device *i2c_bus[4] = {ad1110_i2c_bus_1,
											ad1110_i2c_bus_2,
											RT_NULL,
											RT_NULL};
    rt_uint8_t reg = 0x8c;

    msgs[0].addr  = _addr;     		 /* �ӻ���ַ */
    msgs[0].flags = RT_I2C_WR;       /* д��־ */
    msgs[0].buf   = &reg;            /* ���üĴ���,Ĭ������0x8c*/
    msgs[0].len   = 1;               /* ���������ֽ��� */

    msgs[1].addr  = _addr;     		 /* �ӻ���ַ */
    msgs[1].flags = RT_I2C_RD;       /* ����־ */
    msgs[1].buf   = buf;             /* ��ȡ����ָ�� */
    msgs[1].len   = 3;             	 /* ��ȡ�����ֽ��� */

    if (rt_i2c_transfer(i2c_bus[_ch-1], msgs, 2) == 2)
    {
        return RT_EOK;
    }
    else
    {
        return -RT_ERROR;
    }
}
//AD1110Ӳ����ʼ��
//����ֵ: 0,�ɹ� / -1,�������
int ad1110_hw_init(void)
{
    ad1110_i2c_bus_1 = rt_i2c_bus_device_find(AD1110_I2CBUS_1);  /*����I2C�豸*/
    ad1110_i2c_bus_2 = rt_i2c_bus_device_find(AD1110_I2CBUS_2);

    if (ad1110_i2c_bus_1 == RT_NULL)
    {
        rt_kprintf("can't find ad1110 %s device\r\n", AD1110_I2CBUS_1);
        return -RT_ERROR;
    }
    ADDEBUG("ad1110 set i2c bus to %s\r\n", AD1110_I2CBUS_1);
    if (ad1110_i2c_bus_2 == RT_NULL)
    {
    	rt_kprintf("can't find ad1110 %s device\r\n", AD1110_I2CBUS_2);
        return -RT_ERROR;
    }
    ADDEBUG("ad1110 set i2c bus to %s\r\n", AD1110_I2CBUS_2);

    return RT_EOK;
}
INIT_APP_EXPORT(ad1110_hw_init);
