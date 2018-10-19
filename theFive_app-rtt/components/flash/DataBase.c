#include <string.h>
#include <stdlib.h>
#include "DataBase.h"
#include "flash_port.h"

static struct flash_para_t flash_para;
/**
 * Get current using data section address.
 *
 * @return current using data section address
 */
static uint32_t get_cur_using_data_bak(void)
{
    return flash_para.cur_using_data_bak;
}
/**
 * Set current using data section address.
 *
 * @param using_data_addr current using data section address
 */
static void set_cur_using_data_bak(uint32_t using_data_bak)
{
	flash_para.cur_using_data_bak = using_data_bak;
}

/**
 *	保存正在使用的sector位置
 */
EfErrCode save_cur_using_data_bak(rt_uint32_t cur_data_addr)
{
	read_flash_to_cache(flash_para.flash_start_addr);
	write_cache(flash_para.flash_start_addr, &cur_data_addr, 4);
	save_cache_to_flash(flash_para.flash_start_addr);

	return 0;
}
size_t data_get_used_size(void) {
    size_t header_total_num = 0, physical_size = 0;

    if (flash_para.flash_start_addr < flash_para.flash_end_addr) {
        physical_size = flash_para.flash_end_addr - flash_para.flash_start_addr;
    } else {
        physical_size = LOG_AREA_SIZE - (flash_para.flash_start_addr - flash_para.flash_end_addr);
    }

    header_total_num = physical_size / ERASE_MIN_SIZE + 1;

    return physical_size - header_total_num * SECTOR_HEADER_SIZE;
}

/**
 *	保存样本数据到 flash
 *
 */
EfErrCode save_sample_data(const void *_buffer, rt_size_t _size)
{
	EfErrCode _ret = F_NO_ERR;
	rt_uint32_t cur_using_addr_bak = get_cur_using_data_bak();
    rt_uint32_t data_sec_end_addr;
    size_t sector_used_size = 0;

    if (_size % 4 != 0)
    {
    	_size = (_size / 4 + 1) * 4;
    }
    _ret = read_flash_to_cache(get_cur_using_data_bak());
    data_sec_end_addr = get_header_used_size();
    if(data_sec_end_addr + _size > ERASE_MIN_SIZE)
    {
    	sector_used_size = ERASE_MIN_SIZE - data_sec_end_addr;
    	/*如果sector_used_size = 0，刚好一块scetion用完*/
    	if(sector_used_size)
    	{
    		write_cache(data_sec_end_addr, _buffer, sector_used_size);
        	set_header_used_size(ERASE_MIN_SIZE);
          	_ret = save_cache_to_flash(cur_using_addr_bak);
    	}
    	set_cur_using_data_bak(cur_using_addr_bak + 1);
    	sector_used_size = data_sec_end_addr + _size - ERASE_MIN_SIZE;
    	save_cur_using_data_bak(cur_using_addr_bak + 1);
    	data_sec_end_addr = 4;		//存储从每块的第4个位置开始
    }
    else
    	sector_used_size = _size;

    write_cache(data_sec_end_addr, _buffer, sector_used_size);
	set_header_used_size(data_sec_end_addr + sector_used_size);
  	_ret = save_cache_to_flash(cur_using_addr_bak);

	return _ret;
}
/**
 *	读取样本数据
 */
EfErrCode read_sample_data(const void *_buffer, rt_size_t _size)
{

	return 0;
}
/**
 *  设置系统参数
 */
EfErrCode set_system_para(const void *_buffer, rt_size_t _size)
{
	EfErrCode _ret = F_NO_ERR;

	_ret = read_flash_to_cache(flash_para.flash_start_addr);
	write_cache(SYSTEM_PARAM_PART_INDEX_ADDR*4, _buffer, _size);
  	_ret = save_cache_to_flash(flash_para.flash_start_addr);

	return _ret;
}
/**
 *  读取系统参数
 */
EfErrCode get_system_para(struct system_para_t *_system_paras)
{
	EfErrCode _ret = F_NO_ERR;

	return _ret;
}
/**
 *
 */
EfErrCode ef_flash_set_default(void)
{
	EfErrCode _ret = F_NO_ERR;
/**
 * 	flash_cache[SECTION_USED_SIZE] = 20;
 *	flash_cache[DATA_PARAM_PART_INDEX_ADDR] = 2;
 *	flash_cache[LOG_PARAM_PART_INDEX_ADDR] = flash_para.sector_count-2;
 *	flash_cache[IAP_PARAM_PART_INDEX_ADDR] = flash_para.sector_count-1;
 */
	rt_uint32_t w_buf[4] = {20, 2, flash_para.sector_count-2, flash_para.sector_count-1};

	write_cache(flash_para.flash_start_addr, w_buf, 4*4);
	_ret = save_cache_to_flash(flash_para.flash_start_addr);

	return _ret;
}
EfErrCode load_flash(void)
{
	EfErrCode _ret = F_NO_ERR;
	struct rt_device_blk_geometry _geometry;
	rt_uint32_t _temp = 0;

	flash_para.flash_start_addr = 0;
	get_flash_geometry(&_geometry);
	flash_para.bytes_per_sector = _geometry.bytes_per_sector;
	flash_para.sector_count = _geometry.sector_count;
	//flash_erase(flash_para.flash_start_addr, flash_para.sector_count );
	/* read current using data section address */
	read_flash_to_cache(flash_para.flash_start_addr);
	/* if ENV is not initialize or flash has dirty data, set default for it */
	if (get_header_used_size() == 0xFFFF)
	{
        /* set default ENV */
		_ret = ef_flash_set_default();
	}
	read_cache(flash_para.flash_start_addr, &_temp, 4);
	set_cur_using_data_bak(_temp);

	read_flash_to_cache(get_cur_using_data_bak());

	if (get_header_used_size() == 0xFFFF)
	{
        /* set default ENV */
		set_header_used_size(4);
		set_header_used_times(0);
		save_cache_to_flash(get_cur_using_data_bak());
	}

	return _ret;
}

void flash_test_demo(void)
{
	rt_uint8_t data[11] = {1,2,3,4,5,6,7,8,9,0};
	uint32_t start_time, time_cast;

	start_time = rt_tick_get();
	save_sample_data(data, 12);

    time_cast = rt_tick_get() - start_time;
    EF_DEBUG("write success, total time: %d.%03dS.\n", time_cast / RT_TICK_PER_SECOND,
            time_cast % RT_TICK_PER_SECOND / ((RT_TICK_PER_SECOND * 1 + 999) / 1000));
}
