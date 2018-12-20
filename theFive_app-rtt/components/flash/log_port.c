#include <rthw.h>
#include <rtthread.h>
#include "flash_cfg.h"


//#define Flash_DEBUG
#define LOG_TAG             "flash.log"
#include <flash_log.h>

/* the stored logs start address and end address. It's like a ring buffer implemented on flash. */
static uint32_t log_start_addr = 0, log_end_addr = 0;
/* saved log area address for flash */
static uint32_t log_area_start_addr = 0;
/* initialize OK flag */
static bool init_ok = false;
static rt_uint8_t cur_using_sector;

static void find_cur_using_sector(void);

/**
 * The flash save log function initialize.
 *
 * @return result
 */
EfErrCode ef_log_init(void) {
    EfErrCode result = F_NO_ERR;

    RT_ASSERT(LOG_AREA_SIZE);
    RT_ASSERT(ERASE_MIN_SIZE);
    /* the log area size must be an integral multiple of erase minimum size. */
    RT_ASSERT(LOG_AREA_SIZE % ERASE_MIN_SIZE == 0);
    /* the log area size must be more than twice of ERASE_MIN_SIZE */
    RT_ASSERT(LOG_AREA_SIZE / ERASE_MIN_SIZE >= 2);

    log_area_start_addr = LOG_START_ADDR;

    /* find the log store start address and end address */
    find_cur_using_sector();
    /* initialize OK */
    init_ok = true;

    return result;
}
/**
 * Find the current flash sector using end address by continuous 0xFF.
 *
 * @param addr sector address
 *
 * @return current flash sector using end address
 */
static void find_cur_using_sector(void)
{
	size_t sec_total_num = LOG_AREA_SIZE / ERASE_MIN_SIZE;

	for(rt_uint8_t i=0; i < sec_total_num; i++)
	{
		read_flash_to_cache(log_area_start_addr + i);
		log_end_addr = get_header_used_size();
		if(log_end_addr < ERASE_MIN_SIZE)
		{
			cur_using_sector = log_area_start_addr + i;
			if(get_header_used_times() > 1)
				log_start_addr = log_end_addr;
			else
				log_start_addr = log_area_start_addr + 4;
			break;
		}
	}

}

//EfErrCode log_set_default(void)
//{
//	EfErrCode _ret = F_NO_ERR;
//
//    read_flash_to_cache(cur_using_sector);
//    read_cache(log_area_start_addr, &sector_header, 4);
//	if (get_header_used_size() == 0xFFFF)
//	{
//        /* set default ENV */
//		_ret = ef_flash_set_default();
//	}
//
//	return _ret;
//}
/**
 * Get log used flash total size.
 *
 * @return log used flash total size. @note NOT contain sector headers
 */
size_t log_get_used_size(void) {
    size_t header_total_num = 0, physical_size = 0;
    /* must be call this function after initialize OK */
    if (!init_ok) {
        return 0;
    }

    if (log_start_addr < log_end_addr) {
        physical_size = log_end_addr - log_start_addr;
    } else {
        physical_size = LOG_AREA_SIZE - (log_start_addr - log_end_addr);
    }

    header_total_num = physical_size / ERASE_MIN_SIZE + 1;

    return physical_size - header_total_num * SECTOR_HEADER_SIZE;
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
static EfErrCode log_seq_read(rt_uint32_t addr, rt_uint32_t *log, size_t size) {
    EfErrCode result = F_NO_ERR;
    size_t read_size = 0, read_size_temp = 0;

    while (size) {
        /* move to sector data address */
        if ((addr + read_size) % ERASE_MIN_SIZE == 0) {
            addr += SECTOR_HEADER_SIZE;
        }
        /* calculate current sector last data size */
        read_size_temp = ERASE_MIN_SIZE - (addr % ERASE_MIN_SIZE);
        if (size < read_size_temp) {
            read_size_temp = size;
        }
        result = read_flash_to_cache(addr + read_size);
        if (result != F_NO_ERR) {
            return result;
        }
        read_cache(addr + read_size, log + read_size / 4, read_size_temp);
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
static rt_uint32_t log_index2addr(size_t index) {
    size_t header_total_offset = 0;
    /* total include sector number */
    size_t sector_num = index / (ERASE_MIN_SIZE - SECTOR_HEADER_SIZE) + 1;

    header_total_offset = sector_num * SECTOR_HEADER_SIZE;
    if (log_start_addr < log_end_addr) {
        return log_start_addr + index + header_total_offset;
    }
    else
    {
        if (log_start_addr + index + header_total_offset < log_area_start_addr + LOG_AREA_SIZE) {
            return log_start_addr + index + header_total_offset;
        }
        else
        {
            return log_start_addr + index + header_total_offset - LOG_AREA_SIZE;
        }
    }
}
/**
 * Read log from flash.
 *
 * @param index index for saved log.
 *        Minimum index is 0.
 *        Maximum index is ef_log_get_used_size() - 1.
 * @param log the log which will read from flash
 * @param size read bytes size
 *
 * @return result
 */
EfErrCode ef_log_read(size_t index, rt_uint32_t *log, size_t size) {
    EfErrCode result = F_NO_ERR;
    size_t cur_using_size = log_get_used_size();
    size_t read_size_temp = 0;
    size_t header_total_num = 0;

    if (!size) {
        return result;
    }

    RT_ASSERT(size % 4 == 0);
    RT_ASSERT(index < cur_using_size);

    if (index + size > cur_using_size) {
        LOG_W("Warning: Log read size out of bound. Cut read size.\n");
        size = cur_using_size - index;
    }
    /* must be call this function after initialize OK */
    if (!init_ok) {
        return F_DATA_INIT_FAILED;
    }

    if (log_start_addr < log_end_addr) {
        log_seq_read(log_index2addr(index), log, size);
    } else {
        if (log_index2addr(index) + size <= log_area_start_addr + LOG_AREA_SIZE) {
            /*                          Flash log area
             *                         |--------------|
             * log_area_start_addr --> |##############|
             *                         |##############|
             *                         |##############|
             *                         |--------------|
             *                         |##############|
             *                         |##############|
             *                         |##############| <-- log_end_addr
             *                         |--------------|
             *      log_start_addr --> |##############|
             *          read start --> |**************| <-- read end
             *                         |##############|
             *                         |--------------|
             *
             * read from (log_start_addr + log_index2addr(index)) to (log_start_addr + index + log_index2addr(index))
             */
            result = log_seq_read(log_index2addr(index), log, size);
        } else if (log_index2addr(index) < log_area_start_addr + LOG_AREA_SIZE) {
            /*                          Flash log area
             *                         |--------------|
             * log_area_start_addr --> |**************| <-- read end
             *                         |##############|
             *                         |##############|
             *                         |--------------|
             *                         |##############|
             *                         |##############|
             *                         |##############| <-- log_end_addr
             *                         |--------------|
             *      log_start_addr --> |##############|
             *          read start --> |**************|
             *                         |**************|
             *                         |--------------|
             * read will by 2 steps
             * step1: read from (log_start_addr + log_index2addr(index)) to flash log area end address
             * step2: read from flash log area start address to read size's end address
             */
            read_size_temp = (log_area_start_addr + LOG_AREA_SIZE) - log_index2addr(index);
            header_total_num = read_size_temp / ERASE_MIN_SIZE;
            /* Minus some ignored bytes */
            read_size_temp -= header_total_num * SECTOR_HEADER_SIZE;
            result = log_seq_read(log_index2addr(index), log, read_size_temp);
            if (result == F_NO_ERR) {
                result = log_seq_read(log_area_start_addr, log + read_size_temp / 4, size - read_size_temp);
            }
        } else {
            /*                          Flash log area
             *                         |--------------|
             * log_area_start_addr --> |##############|
             *          read start --> |**************|
             *                         |**************| <-- read end
             *                         |--------------|
             *                         |##############|
             *                         |##############|
             *                         |##############| <-- log_end_addr
             *                         |--------------|
             *      log_start_addr --> |##############|
             *                         |##############|
             *                         |##############|
             *                         |--------------|
             * read from (log_start_addr + log_index2addr(index) - LOG_AREA_SIZE) to read size's end address
             */
            result = log_seq_read(log_index2addr(index) - LOG_AREA_SIZE, log, size);
        }
    }

    return result;
}
/**
 * Write log to flash.
 *
 * @param log the log which will be write to flash
 * @param size write bytes size
 *
 * @return result
 */
EfErrCode ef_log_write(const rt_uint32_t *log, size_t size) {
    EfErrCode _ret = F_NO_ERR;
    size_t write_size = 0, writable_size = 0;
    rt_uint32_t sector_header, write_addr = log_end_addr, erase_addr;

    RT_ASSERT(size % 4 == 0);
    /* must be call this function after initialize OK */
    if (!init_ok) {
        return F_DATA_INIT_FAILED;
    }
    read_flash_to_cache(cur_using_sector);
    if(log_end_addr + size > ERASE_MIN_SIZE)
    {
    	writable_size = ERASE_MIN_SIZE - log_end_addr;
    	if(writable_size)
    	{
    		write_cache(log_end_addr, log, writable_size);
        	set_header_used_size(ERASE_MIN_SIZE);
          	_ret = fresh_cache_to_flash(cur_using_sector);
    	}/*如果writable_size = 0，刚好一块scetion用完*/
    	cur_using_sector += 1;
    	writable_size = log_end_addr + size - ERASE_MIN_SIZE;
    	log_end_addr = 4;		//存储从每块的第4个位置开始
    }
    else
    	writable_size = size;

    write_cache(log_end_addr, log, writable_size);
	set_header_used_size(log_end_addr + writable_size);
  	_ret = fresh_cache_to_flash(cur_using_sector);
  	log_end_addr += writable_size;
  	if(log_end_addr < log_start_addr)
  		log_start_addr = log_end_addr + 1;

  	return _ret;
}
/**
 * Clean all log which in flash.
 *
 * @return result
 */
EfErrCode ef_log_clean(void)
{
    EfErrCode result = F_NO_ERR;

    /* clean address */
    log_start_addr = log_area_start_addr;
    log_end_addr = log_start_addr + SECTOR_HEADER_SIZE;
    /* erase log flash area */
    result = flash_erase(log_area_start_addr/ERASE_MIN_SIZE, LOG_AREA_SIZE/ERASE_MIN_SIZE);
    if (result != F_NO_ERR) {
        goto exit;
    }
exit:
    return result;
}

