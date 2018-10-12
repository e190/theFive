#include <string.h>
#include <stdlib.h>
#include "DataBase.h"
#include "flash_port.h"

static rt_uint32_t flash_cache[ERASE_MIN_SIZE / 4] = { 0 };
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
 * Get ENV detail part end address.
 * It's the first word in ENV.
 *
 * @return ENV end address
 */
static uint32_t get_data_detail_end_addr(void) {
    /* it is the first word */
    return flash_para.data_detail_addr;
}

/**
 * Set ENV detail part end address.
 * It's the first word in ENV.
 *
 * @param end_addr ENV end address
 */
static void set_data_detail_end_addr(uint32_t end_addr) {
	flash_para.data_detail_addr = end_addr;
}

/**
 *	保存正在使用的sector位置
 */
EfErrCode save_cur_using_data_bak(rt_uint32_t cur_data_addr)
{
	flash_read_sector(flash_para.flash_start_addr, flash_cache);
	flash_cache[DATA_PARAM_PART_INDEX_ADDR] = cur_data_addr;
	flash_write_sector(flash_para.flash_start_addr, flash_cache);

	return 0;
}
/**
 *	保存样本数据到 flash
 *
 */
EfErrCode save_sample_data(const void *_buffer, rt_size_t _size)
{
	EfErrCode _ret = F_NO_ERR;
	rt_uint32_t cur_using_addr_bak = get_cur_using_data_bak();
    rt_uint32_t data_sec_end_addr = get_data_detail_end_addr();
    char *flash_cache_bak = (char *)flash_cache;
    size_t sector_used_size = 0;

    if (_size % 4 != 0)
    {
    	_size = (_size / 4 + 1) * 4;
    }
    _ret = flash_read_sector(get_cur_using_data_bak(), flash_cache);
    if(data_sec_end_addr + _size > ERASE_MIN_SIZE)
    {
    	sector_used_size = ERASE_MIN_SIZE - data_sec_end_addr;
    	/*如果sector_used_size = 0，刚好一块scetion用完*/
    	if(sector_used_size)
    	{
        	memcpy(&flash_cache_bak[data_sec_end_addr], _buffer, data_sec_end_addr);
        	flash_cache[SECTION_USED_SIZE] = ERASE_MIN_SIZE;
        	_ret = flash_write_sector(cur_using_addr_bak, flash_cache);
    	}
    	set_cur_using_data_bak(cur_using_addr_bak + 1);
    	sector_used_size = data_sec_end_addr + _size - ERASE_MIN_SIZE;
    	save_cur_using_data_bak(cur_using_addr_bak + 1);
    	data_sec_end_addr = 4;		//存储从每块的第4个位置开始
    }
    else
    	sector_used_size = _size;
    memcpy(&flash_cache_bak[data_sec_end_addr], _buffer, sector_used_size);
    flash_cache[SECTION_USED_SIZE] = data_sec_end_addr + sector_used_size;
	_ret = flash_write_sector(get_cur_using_data_bak(), flash_cache);
	set_data_detail_end_addr(flash_cache[SECTION_USED_SIZE]);

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
	char *flash_cache_bak = (char *)flash_cache;

	_ret = flash_read_sector(flash_para.flash_start_addr, flash_cache);
	memcpy(&flash_cache_bak[SYSTEM_PARAM_PART_INDEX_ADDR], _buffer, _size);
	flash_cache[SECTION_USED_SIZE] = _size + 12;
	_ret = flash_write_sector(flash_para.flash_start_addr, flash_cache);

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

	flash_cache[DATA_PARAM_PART_INDEX_ADDR] = 2;
	flash_cache[LOG_PARAM_PART_INDEX_ADDR] = flash_para.sector_count-2;
	flash_cache[IAP_PARAM_PART_INDEX_ADDR] = flash_para.sector_count-1;

	flash_cache[SECTION_USED_SIZE] = 20;
	_ret = flash_write_sector(flash_para.flash_start_addr, flash_cache);

	return _ret;
}
EfErrCode load_flash(void)
{
	EfErrCode _ret = F_NO_ERR;
	struct rt_device_blk_geometry _geometry;

	flash_para.flash_start_addr = 0;
	get_flash_geometry(&_geometry);
	flash_para.bytes_per_sector = _geometry.bytes_per_sector;
	flash_para.sector_count = _geometry.sector_count;
	//flash_erase(flash_para.flash_start_addr, flash_para.sector_count );
	/* read current using data section address */
	flash_read_sector(flash_para.flash_start_addr, flash_cache);
	/* if ENV is not initialize or flash has dirty data, set default for it */
	if (flash_cache[SECTION_USED_SIZE] == 0xFFFFFFFF)
	{
        /* set default ENV */
		_ret = ef_flash_set_default();
	}
	set_cur_using_data_bak(flash_cache[DATA_PARAM_PART_INDEX_ADDR]);

	flash_read_sector(get_cur_using_data_bak(), flash_cache);

	if (flash_cache[SECTION_USED_SIZE] == 0xFFFFFFFF)
	{
        /* set default ENV */
		flash_cache[SECTION_USED_SIZE] = 4;
	}
	set_data_detail_end_addr(flash_cache[SECTION_USED_SIZE]);

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
