#include "SaveData.h"

extern rt_device_t w25q128; 

rt_size_t write_date(rt_uint16_t _addr)
{
	uint32_t addr = 5;
	rt_uint8_t data[64] = {5,9,8,7,6,5,1,23,4};
	
	return rt_device_write(w25q128, addr, data, 10);	
}

int read_date(rt_uint16_t _addr)
{

}

