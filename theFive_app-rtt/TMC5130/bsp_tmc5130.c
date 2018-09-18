#include "board.h"
#include "bsp_tmc5130.h"
#include "TMC5130.h"

static struct rt_spi_device  spi_dev_tmc5130;

static struct tmc5130_hw_spi_cs  spi_cs;  /* SPI设备CS片选引脚 */
void test_tmc5130(const rt_uint8_t* c)
{
	rt_size_t len = 0;
	
	len = rt_spi_send(&spi_dev_tmc5130, c, 1);
	rt_kprintf("write spi: %d\n",len);
}

int tmc5130_VMax;
rt_uint8_t tmc5130_VMaxModified;
int tmc5130_AMax;
rt_uint8_t tmc5130_AMaxModified;

/* 切换大小端 */
//static rt_int32_t changeEndian(rt_int32_t value){
//	return (value & 0x000000FFU) << 24 | (value & 0x0000FF00U) << 8 | 
//		(value & 0x00FF0000U) >> 8 | (value & 0xFF000000U) >> 24; 
//}

int tmc5130_spi_readInt(rt_uint8_t address)
{
	int value = 0;
	rt_uint8_t temp[5] = {address,0,0,0,0};
	
	rt_spi_transfer(&spi_dev_tmc5130, &temp[0], RT_NULL, 5);
	//__nop();
	//__nop();
	//__nop();
	rt_spi_transfer(&spi_dev_tmc5130, &address, &value, 5);
	
	return (value & 0x000000FFU) << 24 | (value & 0x0000FF00U) << 8 | 
		(value & 0x00FF0000U) >> 8 | (value & 0xFF000000U) >> 24; 
}
void tmc5130_spi_writeInt(rt_uint8_t address, rt_int32_t value)
{
	rt_uint8_t temp[5] = {address | 0x80, 0xFF&(value>>24), 0xFF&(value>>16), 0xFF&(value>>8), 0xFF&(value>>0)};
	rt_spi_transfer(&spi_dev_tmc5130, &temp[0], RT_NULL, 5);
}
int rt_tmc5130_init(void)
{
    struct rt_spi_configuration cfg;
	rt_uint8_t c = 1;
	
	/* oled use PC8 as CS */
    spi_cs.pin = TMC5130_CS_PIN;
    rt_pin_mode(spi_cs.pin, PIN_MODE_OUTPUT);    /* 设置片选管脚模式为输出 */
	//stm32_spi_bus_attach_device(TMC5130_CS_PIN, TMC5130_SPI_BUS_NAME, "tmc5130");
	rt_spi_bus_attach_device(&spi_dev_tmc5130, "tmc5130", TMC5130_SPI_BUS_NAME, (void *)&spi_cs);
	/* config spi */         
	cfg.data_width = 8;
	cfg.mode = RT_SPI_MODE_3 | RT_SPI_MSB; /* SPI Compatible: Mode 0 and Mode 3 */
	cfg.max_hz = 6000 * 1000; /* 300K */
	rt_spi_configure(&spi_dev_tmc5130, &cfg);
	
	rt_spi_send(&spi_dev_tmc5130, &c, 1);
	rt_thread_delay(50);
	tmc5130_init();
    return 0;
}
INIT_DEVICE_EXPORT(rt_tmc5130_init);
