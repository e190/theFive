#ifndef __UART_SCREEN_H__
#define __UART_SCREEN_H__

#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>



#define TMC5130_CS_PIN 116
#define TMC5130_SPI_BUS_NAME "spi3"

struct tmc5130_hw_spi_cs
{
    rt_uint32_t pin;
};

void test_tmc5130(const rt_uint8_t* c);
void tmc5130_spi_writeInt(rt_uint8_t address, rt_int32_t value);
int tmc5130_spi_readInt(rt_uint8_t address);

#endif

