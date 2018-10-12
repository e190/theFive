#ifndef FLASH_PORT_H_
#define FLASH_PORT_H_
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sfud.h>
#include <rthw.h>
#include <rtthread.h>
#include "flash_cfg.h"

/* ef_port.c */
EfErrCode ef_port_read(uint32_t addr, uint32_t *buf, size_t size);
EfErrCode ef_port_erase(uint32_t addr, size_t size);
EfErrCode ef_port_write(uint32_t addr, const uint32_t *buf, size_t size);
void ef_log_debug(const char *file, const long line, const char *format, ...);
void ef_log_info(const char *format, ...);
void ef_print(const char *format, ...);

EfErrCode flash_write_sector(rt_uint32_t _addr, const void *_buffer);
EfErrCode flash_read_sector(rt_uint32_t _addr, void *_buffer);
EfErrCode flash_erase(uint32_t addr, size_t size);
EfErrCode get_flash_geometry(struct rt_device_blk_geometry *flash_geometry);

#endif /* FLASH_PORT_H_ */
