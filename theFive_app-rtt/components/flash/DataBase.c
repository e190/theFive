#include <rthw.h>
#include <rtthread.h>
#include "DataBase.h"
#include "flash_port.h"

static struct flash_para_t flash_para;
/**
 * Get current using data section address.
 *
 * @return current using data section address
 */
static rt_uint32_t get_cur_using_data_bak(void)
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
	read_flash_to_cache(0);
	write_cache(DATA_PARAM_PART_INDEX_ADDR, &cur_data_addr, 4);
	save_cache_to_flash(0);

	return 0;
}
/**
 *	获取已使用的空间
 */
size_t data_get_used_size(void)
{
    size_t header_total_num = 0, physical_size = 0;
    rt_uint32_t sec_start = flash_para.data_start_addr/ERASE_MIN_SIZE;
    rt_uint32_t sec_end = flash_para.data_end_addr/ERASE_MIN_SIZE;

    if (flash_para.data_start_addr < flash_para.data_end_addr) {
        physical_size = flash_para.data_end_addr - flash_para.data_start_addr;
        header_total_num = sec_end - sec_start;
    } else {
        physical_size = LOG_AREA_SIZE - (flash_para.data_start_addr - flash_para.data_end_addr);
        header_total_num = sec_start - sec_end;
    }

    return physical_size - header_total_num * SECTOR_HEADER_SIZE;
}
/**
 * Find the current flash sector using end address by continuous 0xFF.
 *
 * @param addr sector address
 *
 * @return current flash sector using end address
 */
static void find_start_end_addr(void)
{
	size_t sec_total_num = DATA_AREA_SIZE / ERASE_MIN_SIZE;
	rt_uint8_t data_start_sec = DATA_START_ADDR / ERASE_MIN_SIZE;
	rt_uint32_t remainder = (DATA_AREA_SIZE - sec_total_num*4) % flash_para.size_per_index;
	rt_uint32_t sec_header_size;

	for(rt_uint8_t i=0; i < sec_total_num; i++)
	{
		read_flash_to_cache(data_start_sec + i);
		sec_header_size = get_header_used_size();
		if(sec_header_size < ERASE_MIN_SIZE)
		{
			flash_para.data_end_addr = DATA_START_ADDR + sec_header_size + i * ERASE_MIN_SIZE;
			if(get_header_used_times() > 1)
			{
				flash_para.data_start_addr = flash_para.data_end_addr + remainder;
				while((flash_para.data_start_addr/ERASE_MIN_SIZE) == (flash_para.data_end_addr/ERASE_MIN_SIZE))
				{
					flash_para.data_start_addr += remainder;
				}
			}
			else//第一次循环
				flash_para.data_start_addr = DATA_START_ADDR + 4;
			break;
		}
	}

}
/**
 *	获取已保存的条目数
 */
size_t data_get_used_index(void)
{
	EF_ASSERT(flash_para.size_per_index);
    return data_get_used_size() / flash_para.size_per_index;
}
/**
 * Sequential reading log data. It will ignore sector headers.
 *
 * @param addr address
 * @param log log buffer
 * @param size log size, not contain sector headers.
 *
 * @return result
 */
static EfErrCode data_seq_read(rt_uint32_t addr, rt_uint32_t *buf, size_t size)
{
    EfErrCode result = F_NO_ERR;
    size_t read_size = 0, read_size_temp = 0;

    while (size)
    {
        /* move to sector data address */
        if ((addr + read_size) % ERASE_MIN_SIZE == 0) {
            addr += SECTOR_HEADER_SIZE;
        }
        /* calculate current sector last data size */
        read_size_temp = ERASE_MIN_SIZE - ((addr + read_size) % ERASE_MIN_SIZE);
        if (size < read_size_temp) {
            read_size_temp = size;
        }
        result = read_flash_to_cache((addr + read_size) / ERASE_MIN_SIZE);
        if (result != F_NO_ERR) {
            return result;
        }
        read_cache((addr + read_size) % ERASE_MIN_SIZE, buf + read_size / 4, read_size_temp);
        read_size += read_size_temp;
        size -= read_size_temp;
    }

    return result;
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
    if (flash_para.data_start_addr < flash_para.data_end_addr) {
        return DATA_START_ADDR + data_offset + header_total_offset;
    }
    else
    {
        if (flash_para.data_start_addr + data_offset + header_total_offset < DATA_START_ADDR + DATA_AREA_SIZE) {
            return flash_para.data_start_addr + data_offset + header_total_offset;
        }
        else
        {
            return flash_para.data_start_addr + data_offset + header_total_offset - DATA_AREA_SIZE;
        }
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
    size_t cur_using_index = data_get_used_index();
    size_t read_size = 0;
    size_t header_total_num = 0;

    if (!size) {
        return result;
    }

    EF_ASSERT(index < cur_using_index);

    if (index + size > cur_using_index) {
        EF_DEBUG("Warning: Log read size out of bound. Cut read size.\n");
        size = cur_using_index - index;
    }
    /* must be call this function after initialize OK */
    if (!flash_para.init_ok) {
        return F_DATA_INIT_FAILED;
    }
    read_size = size * flash_para.size_per_index;
    if (flash_para.data_start_addr < flash_para.data_end_addr) {
        data_seq_read(data_index2addr(index), _buffer, read_size);
    }
    else
    {
        if (data_index2addr(index) + read_size <= flash_para.data_start_addr + DATA_AREA_SIZE) {
            /*                          Flash data area
             *                         |--------------|
             *data_area_start_addr --> |##############|
             *                         |##############|
             *                         |##############|
             *                         |--------------|
             *                         |##############|
             *                         |##############|
             *                         |##############| <-- data_end_addr
             *                         |--------------|
             *     data_start_addr --> |##############|
             *          read start --> |**************| <-- read end
             *                         |##############|
             *                         |--------------|
             *
             * read from (log_start_addr + log_index2addr(index)) to (log_start_addr + index + log_index2addr(index))
             */
            result = data_seq_read(data_index2addr(index), _buffer, read_size);
        }
        else if (data_index2addr(index) < flash_para.data_start_addr + DATA_AREA_SIZE)
        {
            /*                          Flash data area
             *                         |--------------|
             *data_area_start_addr --> |**************| <-- read end
             *                         |##############|
             *                         |##############|
             *                         |--------------|
             *                         |##############|
             *                         |##############|
             *                         |##############| <-- data_end_addr
             *                         |--------------|
             *     data_start_addr --> |##############|
             *          read start --> |**************|
             *                         |**************|
             *                         |--------------|
             * read will by 2 steps
             * step1: read from (log_start_addr + log_index2addr(index)) to flash log area end address
             * step2: read from flash log area start address to read size's end address
             */
        	size_t read_size_temp = (flash_para.data_start_addr + DATA_AREA_SIZE) - data_index2addr(index);
            header_total_num = read_size_temp / ERASE_MIN_SIZE;
            /* Minus some ignored bytes */
            read_size_temp -= header_total_num * SECTOR_HEADER_SIZE;
            result = data_seq_read(data_index2addr(index), _buffer, read_size_temp);
            if (result == F_NO_ERR) {
                result = data_seq_read(flash_para.data_start_addr, _buffer + read_size_temp / 4, read_size - read_size_temp);
            }
        }
        else {
              /*                          Flash data area
               *                         |--------------|
               *data_area_start_addr --> |##############|
               *          read start --> |**************|
               *                         |**************| <-- read end
               *                         |--------------|
               *                         |##############|
               *                         |##############|
               *                         |##############| <-- data_end_addr
               *                         |--------------|
               *     data_start_addr --> |##############|
               *                         |##############|
               *                         |##############|
               *                         |--------------|
               * read from (log_start_addr + log_index2addr(index) - LOG_AREA_SIZE) to read size's end address
               */
            result = data_seq_read(data_index2addr(index) - DATA_AREA_SIZE, _buffer, read_size);
        }
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
	rt_uint32_t cur_using_addr_bak = get_cur_using_data_bak();
	char *char_buffer = (char *)_buffer;
    rt_uint32_t data_sec_end_addr;
    size_t sector_used_size = 0;

    if (_size % 4 != 0)
    {
    	_size = (_size / 4 + 1) * 4;
    }
    _ret = read_flash_to_cache(cur_using_addr_bak);
    data_sec_end_addr = get_header_used_size();
    if(data_sec_end_addr + _size >= ERASE_MIN_SIZE)
    {
    	sector_used_size = ERASE_MIN_SIZE - data_sec_end_addr;
    	if(sector_used_size)
    	{
    		write_cache(data_sec_end_addr, char_buffer, sector_used_size);
        	set_header_used_size(ERASE_MIN_SIZE);
          	_ret = save_cache_to_flash(cur_using_addr_bak);
          	flash_para.data_end_addr += sector_used_size;
          	char_buffer += sector_used_size;
          	_size -= sector_used_size;
    	}/*如果sector_used_size = 0，刚好一块scetion用完*/
    	cur_using_addr_bak += 1;
    	if(cur_using_addr_bak >= (DATA_START_ADDR+DATA_AREA_SIZE) / ERASE_MIN_SIZE)
    		cur_using_addr_bak = DATA_START_ADDR / ERASE_MIN_SIZE;
    	set_cur_using_data_bak(cur_using_addr_bak);	//设置新块的位置
    	save_cur_using_data_bak(cur_using_addr_bak);
    	/*设置当前块的次数信息*/
    	read_flash_to_cache(cur_using_addr_bak);
    	rt_uint16_t times = get_header_used_times();
    	if(times != 0xffff)
    	{
    		set_header_used_times(times+1);
    		rt_uint32_t temp_size = flash_para.data_start_addr % ERASE_MIN_SIZE;
    		if(temp_size % flash_para.size_per_index != 0)
        		temp_size = (temp_size / flash_para.size_per_index + 1) * temp_size;
        	flash_para.data_start_addr += temp_size;
    	}
    	else
    		set_header_used_times(1);
    	flash_erase(cur_using_addr_bak, 1);	//擦除新块
    	sector_used_size = _size;
    	data_sec_end_addr = 4;		//存储从每块的第4个位置开始
    }
    else
    	sector_used_size = _size;

    write_cache(data_sec_end_addr, char_buffer, sector_used_size);
	set_header_used_size(data_sec_end_addr + sector_used_size);
  	_ret = save_cache_to_flash(cur_using_addr_bak);
  	flash_para.data_end_addr += sector_used_size;

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
    flash_para.data_start_addr = DATA_START_ADDR + SECTOR_HEADER_SIZE;
    flash_para.data_end_addr = flash_para.data_start_addr;
    /* erase data flash area */
    _ret = flash_erase(DATA_START_ADDR/ERASE_MIN_SIZE, DATA_AREA_SIZE/ERASE_MIN_SIZE);
    if (_ret != F_NO_ERR) {
    	return _ret;
    }

	set_header_used_size(4);
	set_header_used_times(1);
	_ret = save_cache_to_flash(get_cur_using_data_bak());
    if (_ret != F_NO_ERR) {
    	return _ret;
    }
    return _ret;
}
/**
 *  设置系统参数
 */
EfErrCode set_system_para(const void *_buffer, rt_size_t _size)
{
	EfErrCode _ret = F_NO_ERR;

	_ret = read_flash_to_cache(flash_para.data_start_addr);
	write_cache(SYSTEM_PARAM_PART_INDEX_ADDR*4, _buffer, _size);
  	_ret = save_cache_to_flash(flash_para.data_start_addr);

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

	write_cache(SECTION_USED_SIZE, w_buf, 4*4);
	_ret = save_cache_to_flash(0);

	return _ret;
}
EfErrCode load_flash(void)
{
	EfErrCode _ret = F_NO_ERR;
	struct rt_device_blk_geometry _geometry;
	rt_uint32_t _temp = 0;

	get_flash_geometry(&_geometry);
	flash_para.bytes_per_sector = _geometry.bytes_per_sector;
	flash_para.sector_count = _geometry.sector_count;
	/* read system section address */
	read_flash_to_cache(0);
	/* if ENV is not initialize or flash has dirty data, set default for it */
	if (get_header_used_size() == 0xFFFF)
	{
        /* set default ENV */
		_ret = ef_flash_set_default();
	}
	read_cache(DATA_PARAM_PART_INDEX_ADDR, &_temp, 4); //读出 DATA区的 储存块位置
	set_cur_using_data_bak(_temp);

	read_flash_to_cache(get_cur_using_data_bak());//读出 DATA区的 当前具体位置

	if (get_header_used_size() == 0xFFFF){
		_ret = data_base_clean();
	}

	flash_para.size_per_index = sizeof(struct sample_data_t);
    if (flash_para.size_per_index % 4 != 0)
    {
    	flash_para.size_per_index = (flash_para.size_per_index / 4 + 1) * 4;
    }
    find_start_end_addr();
	flash_para.init_ok = true;

	return _ret;
}
void flash_clean(void)
{
	EF_ASSERT(flash_para.sector_count);
	flash_erase(0, flash_para.sector_count); //擦除全部
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
    EF_DEBUG("write success, total time: %d.%03dS.\n", time_cast / RT_TICK_PER_SECOND,
            time_cast % RT_TICK_PER_SECOND / ((RT_TICK_PER_SECOND * 1 + 999) / 1000));
}
void flash_read_demo(rt_uint8_t num)
{
	struct sample_data_t read_data;

	read_sample_data(num, (rt_uint32_t*)&read_data, 1);
	rt_kprintf("%d  %s", read_data.data[8], (char*)&read_data);
}
