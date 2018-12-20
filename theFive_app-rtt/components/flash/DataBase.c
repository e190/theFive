#include <rthw.h>
#include <rtthread.h>
#include "DataBase.h"
#include "spi_flash_init.h"

#if defined(PKG_USING_FAL)
#include "fal.h"
#endif

#define Flash_DEBUG
#define LOG_TAG   "flash.data"
#include <flash_log.h>

static struct flash_para_t flash_para;
static struct system_para_t system_para;
/**
 * Get current using data section address.
 *
 * @return current using data section address
 */
static rt_uint32_t get_cur_using_data_bak(void)
{

	fal_partition_read(flash_system, DATA_PARAM_PART_INDEX_ADDR,
					(rt_uint8_t*)&flash_para.cur_using_data_bak, 4);

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
	fal_partition_write(flash_system, DATA_PARAM_PART_INDEX_ADDR,
			(rt_uint8_t*)&cur_data_addr, 4);

	return 0;
}
/**
 *	获取已使用的空间（包含 头 信息）
 */
size_t get_data_used_size(void)
{
    size_t detail_size = 0;

    fal_partition_read(flash_sample, get_cur_using_data_bak(),
            					(rt_uint8_t*)detail_size, 4);

    detail_size += get_cur_using_data_bak() * ERASE_MIN_SIZE;

    return detail_size;
}

/**
 *	获取已保存的条目数
 */
size_t get_data_used_index(void)
{
	RT_ASSERT(flash_para.size_per_index);

	size_t sector_num = get_data_used_size() / ERASE_MIN_SIZE;
	size_t header_total_offset = sector_num * SECTOR_HEADER_SIZE;

    return (get_data_used_size() - header_total_offset) / flash_para.size_per_index;
}
/**
 * Calculate flash physical address by log index.
 *
 * @param index log index
 *
 * @return flash physical address
 */
static rt_uint32_t data_index2addr(size_t index)
{
    size_t header_total_offset = 0;
    size_t data_offset = index * flash_para.size_per_index;
    /* total include sector number */
    size_t sector_num = data_offset / (ERASE_MIN_SIZE - SECTOR_HEADER_SIZE) + 1;
    header_total_offset = sector_num * SECTOR_HEADER_SIZE;

    if (index < get_data_used_index()) {
        return  data_offset + header_total_offset;
    }
    else
    {
    	return 0;
    }
}
/**
 * Read sample data from flash.
 *
 * @param index 开始的条目数
 *        Minimum index is 0.
 *        Maximum index is data_get_used_index() - 1.
 * @param _buffer the data which will read from flash
 * @param size 读数据的条目数
 *
 * @return result
 */
EfErrCode read_sample_data(size_t index, rt_uint32_t *_buffer, size_t size)
{
    EfErrCode result = F_NO_ERR;
    size_t cur_using_index = get_data_used_index();
    size_t read_size = 0;
    size_t header_total_num = 0;

    if (!size) {
        return result;
    }

    RT_ASSERT(index < cur_using_index);

    if (index + size > cur_using_index) {
        LOG_W("Warning: Log read size out of bound. Cut read size.\n");
        size = cur_using_index - index;
    }
    /* must be call this function after initialize OK */
    if (!flash_para.init_ok) {
        return F_DATA_INIT_FAILED;
    }
    read_size = size * flash_para.size_per_index;
    if (flash_para.data_start_addr < flash_para.data_end_addr) {
        fal_partition_read(flash_sample, data_index2addr(index),
        					(rt_uint8_t*)_buffer, read_size);
    }

    return result;
}
/**
 *	保存样本数据到 flash
 *
 */
EfErrCode save_sample_data(const void *_buffer, rt_size_t _size)
{
	EfErrCode _ret = F_NO_ERR;
	rt_uint32_t detail_addr;

	fal_partition_read(flash_sample, get_cur_using_data_bak() * ERASE_MIN_SIZE,
			(rt_uint8_t*)&detail_addr, 4);

	detail_addr += get_cur_using_data_bak() * ERASE_MIN_SIZE;

	fal_partition_write(flash_sample, detail_addr, (rt_uint8_t*)_buffer, _size);

	/* 更新存储位置 */
	save_cur_using_data_bak(0);

	return _ret;
}
/**
 * Clean all log which in flash.
 *
 * @return result
 */
EfErrCode data_base_clean(void)
{
    EfErrCode _ret = F_NO_ERR;

    /* clean address */
    fal_partition_erase_all(flash_sample);
    return _ret;
}
/**
 *  设置系统参数
 */
EfErrCode fresh_system_para(void)
{
	EfErrCode _ret = F_NO_ERR;
  	fal_partition_write(flash_system, SYSTEM_PARAM_PART_INDEX_ADDR, (rt_uint8_t*)&system_para, sizeof(system_para));
	return _ret;
}
/**
 *  读取系统参数
 */
EfErrCode get_system_para(void)
{
	EfErrCode _ret = F_NO_ERR;
	fal_partition_read(flash_system, SYSTEM_PARAM_PART_INDEX_ADDR, (rt_uint8_t*)&system_para, sizeof(system_para));
	return _ret;
}
void set_system_para_cache(rt_off_t pos, const void *_buffer, rt_size_t _size)
{
	rt_uint8_t *system_addr = (rt_uint8_t*)&system_para;
	rt_memcpy(system_addr + pos, _buffer, _size);
}
void get_system_para_cache(rt_off_t pos, void *_buffer, rt_size_t _size)
{
	rt_uint8_t *system_addr = (rt_uint8_t*)&system_para;
	rt_memcpy(_buffer, system_addr + pos, _size);
}
/**
 *
 */
EfErrCode ef_flash_set_default(void)
{
	EfErrCode _ret = F_NO_ERR;
	rt_uint8_t* w_Buf = NULL;
	struct system_para_t _system_para = {
			.sample_para_1 = DEFAULT_PARAM,
			.sample_para_2 = DEFAULT_PARAM,
			.sample_para_3 = DEFAULT_PARAM,
			.sample_para_4 = DEFAULT_PARAM,
			.heat_para_1 = DEFAULT_PID,
			.heat_para_2 = DEFAULT_PID,
			.heat_para_3 = DEFAULT_PID,
			.heat_para_4 = DEFAULT_PID,
			.mix_door_para = {0},
	};
	rt_uint32_t header[4] = {20, 2, flash_para.sector_count-2, flash_para.sector_count-1};

	w_Buf = rt_malloc(sizeof(_system_para) + sizeof(header));
	rt_memcpy(&w_Buf[0], header, sizeof(header));
	rt_memcpy(&w_Buf[16], &_system_para, sizeof(_system_para));

	fal_partition_write(flash_system, 0, (rt_uint8_t*)w_Buf, sizeof(_system_para) + sizeof(header));
	rt_free(w_Buf);

	return _ret;
}
int load_flash(void)
{
	EfErrCode _ret = F_NO_ERR;
	rt_uint32_t _temp = 0;

	/* read system section address */
	get_cur_using_data_bak();
	/* if ENV is not initialize or flash has dirty data, set default for it */
	if (get_cur_using_data_bak() == 0xFFFFFFFF)
	{
        /* set default ENV */
		_ret = ef_flash_set_default();
	}

	/* 读出系统参数  */
	get_system_para();

	/*确定每条样本数据的大小*/
	flash_para.size_per_index = sizeof(struct sample_data_t);
    if (flash_para.size_per_index % 4 != 0)
    {
    	flash_para.size_per_index = (flash_para.size_per_index / 4 + 1) * 4;
    }

	flash_para.init_ok = true;
	LOG_I("load flash success");

	return _ret;
}
void flash_clean(void)
{
	RT_ASSERT(flash_para.sector_count);
	//flash_erase(0, flash_para.sector_count); //擦除全部
}
void flash_test_demo(rt_uint8_t num)
{
	struct sample_data_t test_data = {
			.data = "this is 0 index\n"
	};
	test_data.data[498] = 0xff;
	test_data.data[499] = 0xff;
	rt_uint32_t start_time, time_cast;

	start_time = rt_tick_get();
	test_data.data[8] = num;
	save_sample_data(&test_data, sizeof(struct sample_data_t));

    time_cast = rt_tick_get() - start_time;
    LOG_I("write success, total time: %d.%03dS.\n", time_cast / RT_TICK_PER_SECOND,
            time_cast % RT_TICK_PER_SECOND / ((RT_TICK_PER_SECOND * 1 + 999) / 1000));
}
void flash_read_demo(rt_uint8_t num)
{
	struct sample_data_t read_data;

	read_sample_data(num, (rt_uint32_t*)&read_data, 1);
	rt_kprintf("%d  %s", read_data.data[8], (char*)&read_data);
}
