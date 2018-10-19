/*
 * This file is part of the EasyFlash Library.
 *
 * Copyright (c) 2015, Armink, <armink.ztl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Function: Portable interface for SFUD library.
 * Created on: 2015-01-16
 */

#include "flash_port.h"

extern rt_device_t w25q128;

static rt_uint32_t flash_cache[ERASE_MIN_SIZE / 4] = { 0 };
static char log_buf[RT_CONSOLEBUF_SIZE];

EfErrCode flash_erase(uint32_t addr, size_t size)
{
    EfErrCode result = F_NO_ERR;
    rt_err_t erase_result = RT_EOK;
    rt_uint32_t address[2]={addr, size};

    erase_result = rt_device_control(w25q128, RT_DEVICE_CTRL_BLK_ERASE, address);
    if(erase_result != RT_EOK) {
        result = F_ERASE_ERR;
    }

    return result;
}

EfErrCode get_flash_geometry(struct rt_device_blk_geometry *flash_geometry)
{
	EfErrCode _ret = F_NO_ERR;
	rt_err_t get_result = RT_EOK;

	EF_ASSERT(flash_geometry);
	get_result = rt_device_control(w25q128, RT_DEVICE_CTRL_BLK_GETGEOME, flash_geometry);
	EF_INFO("bytes_per_sector : %d\n",flash_geometry->bytes_per_sector);
	EF_INFO("sector count : %d\n",flash_geometry->sector_count);
	EF_INFO("block size : %d\n",flash_geometry->block_size);
    if(get_result != RT_EOK) {
    	_ret = F_DATA_INIT_FAILED;
    }

	return _ret;
}
/**
 *	读取flash到缓冲区
 */
EfErrCode read_flash_to_cache(rt_uint32_t _addr)
{
    EfErrCode result = F_NO_ERR;
    rt_size_t read_result = 0;

    _addr = _addr % ERASE_MIN_SIZE;
    read_result = rt_device_read(w25q128, _addr, flash_cache, 1);
    if(read_result == 0) {
        result = F_READ_ERR;
    }
	return result;
}
/**
 *	从缓冲区读取数据
 */
EfErrCode read_cache(rt_uint32_t addr, rt_uint32_t *buf, size_t size)
{
    EfErrCode result = F_NO_ERR;
    rt_size_t read_result = 0;
    rt_uint16_t sector_num, detail_size;
	char *flash_cache_bak = (char *)flash_cache;

	detail_size = addr % ERASE_MIN_SIZE;

	rt_memcpy(buf, &flash_cache_bak + detail_size, detail_size);


	return result;
}
/**
 *	写数据到缓冲区
 */
EfErrCode write_cache(rt_uint32_t _sector_offset, const void *_buffer, rt_size_t _size)
{
	EfErrCode _ret = F_NO_ERR;
	uint16_t detail_addr = _sector_offset % ERASE_MIN_SIZE;
	char *flash_cache_bak = (char *)flash_cache;

	rt_memcpy(&flash_cache_bak + detail_addr, _buffer, _size);

	return _ret;
}
/**
 *	写数据到缓冲区
 */
EfErrCode save_cache_to_flash(rt_uint32_t addr)
{
    EfErrCode result = F_NO_ERR;
    rt_size_t write_result = 0;

    write_result = rt_device_write(w25q128, addr, flash_cache, 1);
    if(write_result == 0) {
        result = F_WRITE_ERR;
    }

	return result;
}
rt_size_t get_header_used_size(void)
{
	return flash_cache[SECTION_USED_SIZE] & 0x0000ffff;
}
EfErrCode set_header_used_size(rt_size_t _size)
{
	flash_cache[SECTION_USED_SIZE] |= _size & 0x0000ffff;
	return F_NO_ERR;
}
rt_size_t get_header_used_times(void)
{
	return flash_cache[SECTION_USED_SIZE] >> 16;
}
EfErrCode set_header_used_times(rt_uint32_t _times)
{
	flash_cache[SECTION_USED_SIZE] |= _times << 16;
	return F_NO_ERR;
}

/**
 * This function is print flash debug info.
 *
 * @param file the file which has call this function
 * @param line the line number which has call this function
 * @param format output format
 * @param ... args
 *
 */
void ef_log_debug(const char *file, const long line, const char *format, ...) {

#ifdef FLASH_DEBUG

    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);
    ef_print("[Flash](%s:%ld) ", file, line);
    /* must use vprintf to print */
    rt_vsprintf(log_buf, format, args);
    ef_print("%s", log_buf);
    va_end(args);

#endif

}

/**
 * This function is print flash routine info.
 *
 * @param format output format
 * @param ... args
 */
void ef_log_info(const char *format, ...) {
    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);
    ef_print("[Flash]");
    /* must use vprintf to print */
    rt_vsprintf(log_buf, format, args);
    ef_print("%s", log_buf);
    va_end(args);
}
/**
 * This function is print flash non-package info.
 *
 * @param format output format
 * @param ... args
 */
void ef_print(const char *format, ...) {
    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);
    /* must use vprintf to print */
    rt_vsprintf(log_buf, format, args);
    rt_kprintf("%s", log_buf);
    va_end(args);
}

#if defined(RT_USING_FINSH) && defined(FINSH_USING_MSH) && defined(EF_USING_ENV)
#include <finsh.h>
#if defined(EF_USING_ENV)
void setenv(uint8_t argc, char **argv) {
    uint8_t i;
    char c_value = NULL;
    char *value = &c_value;
    if (argc > 3) {
        /* environment variable value string together */
        for (i = 0; i < argc - 2; i++) {
            argv[2 + i][rt_strlen(argv[2 + i])] = ' ';
        }
    }
    if (argc == 1) {
        ef_set_env(value, value);
    } else if (argc == 2) {
        ef_set_env(argv[1], value);
    } else {
        ef_set_env(argv[1], argv[2]);
    }
}
MSH_CMD_EXPORT(setenv, Set an envrionment variable.);

void printenv(uint8_t argc, char **argv) {
    ef_print_env();
}
MSH_CMD_EXPORT(printenv, Print all envrionment variables.);

void saveenv(uint8_t argc, char **argv) {
    ef_save_env();
}
MSH_CMD_EXPORT(saveenv, Save all envrionment variables to flash.);

void getvalue(uint8_t argc, char **argv) {
    char *value = NULL;
    value = ef_get_env(argv[1]);
    if (value) {
        rt_kprintf("The %s value is %s.\n", argv[1], value);
    } else {
        rt_kprintf("Can't find %s.\n", argv[1]);
    }
}
MSH_CMD_EXPORT(getvalue, Get an envrionment variable by name.);

void resetenv(uint8_t argc, char **argv) {
    ef_env_set_default();
}
MSH_CMD_EXPORT(resetenv, Reset all envrionment variable to default.);
#endif /* defined(EF_USING_ENV) */
#endif /* defined(RT_USING_FINSH) && defined(FINSH_USING_MSH) */
