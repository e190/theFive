#include "SaveFlash.h"

extern rt_device_t w25q128; 

rt_size_t flash_write(rt_uint16_t _addr, const void *_buffer, rt_size_t _size)
{
	uint32_t addr = 5;
	rt_uint8_t data[64] = {5,9,8,7,6,5,1,23,4};
	
	return rt_device_write(w25q128, addr, data, 10);	
}

int flash_read(rt_uint16_t _addr, void *_buffer, rt_size_t _size)
{
	return rt_device_read(w25q128, _addr, _buffer, _size);
}

