#ifndef FLASH_PORT_H_
#define FLASH_PORT_H_
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sfud.h>
#include <rthw.h>
#include <rtthread.h>
#include "flash_cfg.h"

EfErrCode read_flash_to_cache(rt_uint32_t _addr);
EfErrCode read_cache(rt_uint32_t addr, rt_uint32_t *buf, size_t size);
EfErrCode write_cache(rt_uint32_t _sector_offset, const void *_buffer, rt_size_t _size);
EfErrCode save_cache_to_flash(rt_uint32_t addr);
rt_size_t get_header_used_size(void);
EfErrCode set_header_used_size(rt_size_t _size);
rt_size_t get_header_used_times(void);
EfErrCode set_header_used_times(rt_uint32_t _times);

void ef_log_debug(const char *file, const long line, const char *format, ...);
void ef_log_info(const char *format, ...);
void ef_print(const char *format, ...);

EfErrCode flash_erase(uint32_t addr, size_t size);
EfErrCode get_flash_geometry(struct rt_device_blk_geometry *flash_geometry);

#endif /* FLASH_PORT_H_ */
