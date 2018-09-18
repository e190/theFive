#ifndef _BSP_RMD8_H_
#define _BSP_RMD8_H_
#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>

#define RT_PRINTER_DEVICE_NAME  "uart3"

void TestPrinter(void);
int printer_init(void);
int printer_set_font(rt_uint8_t font_size);
int printer_set_gap(rt_uint8_t gap_size);
	
#endif

