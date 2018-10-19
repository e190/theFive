#ifndef __DRV_AD1110_H__
#define __DRV_AD1110_H__

#include <rthw.h>
#include <rtdevice.h>

#if 0
    #define ADDEBUG      rt_kprintf
#else
    #define ADDEBUG(...)
#endif

#define AD1110_I2CBUS_1  		"i2c1"     /* I2C设备名称,必须和drv_i2c.c注册的I2C设备名称一致 */
#define AD1110_I2CBUS_2  		"i2c2"
#define AD1110_I2CBUS_3  		"i2c3"
#define AD1110_I2CBUS_4  		"i2c4"
#define ED0_ADDR                0X94>>1
#define ED2_ADDR                0X90>>1

rt_err_t ad1110_write_reg(rt_uint8_t _addr, rt_uint8_t reg, rt_uint8_t data);
rt_err_t ad1110_read_reg(rt_uint8_t _addr, rt_uint8_t reg, rt_uint8_t *buf);
int ad1110_hw_init(void);

#endif
