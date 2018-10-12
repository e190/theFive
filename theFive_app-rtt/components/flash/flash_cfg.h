/**
 *
 * This all Backup Area Flash storage index. All used flash area configure is under here.
 *	Storage Size ---- 128M bit(16M bytes)
 * |----------------------------|
 * |      system section      	|   system section size(4096)
 * |----------------------------|
 * |      data section        	|   data section size(...)
 * |----------------------------|
 * |      Saved log area        |   Log area size(4096)
 * |----------------------------|
 * |(IAP)Downloaded application |   IAP already downloaded application, unfixed size
 * |----------------------------|
 *
 *		system section
 *		-->section used size (4)
 *		-->data address  (4)
 *		-->log area address (4)
 *		-->IAP	area address (4)
 *		-->system parameters (...)
 *
 */
#ifndef FLASH_CFG_H_
#define FLASH_CFG_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define TOTAL_SIZE		0x1000000	//flash������16mb (0~0x1000000)
/* the minimum size of flash erasure */
#define ERASE_MIN_SIZE  4096

#define SYS_SIZE		0x1000
#define LOG_AREA_SIZE	0x1000
#define IAP_SIZE		0x1000
#define DATA_SIZE		0x100

#define LOG_START_ADDR   0
#define FLASH_DEBUG

/* flash ENV parameters part index and size */
enum {
	SECTION_USED_SIZE = 0,
    /* data section data detail part end address index */
    DATA_PARAM_PART_INDEX_ADDR,
    /* data section CRC32 code index */
    LOG_PARAM_PART_INDEX_ADDR,
    /* ENV parameters part word size */
	IAP_PARAM_PART_INDEX_ADDR,
    /* ENV parameters part byte size */
	SYSTEM_PARAM_PART_INDEX_ADDR,
};

/* EasyFlash debug print function. Must be implement by user. */
#define EF_DEBUG(...) ef_log_debug(__FILE__, __LINE__, __VA_ARGS__)
/* EasyFlash routine print function. Must be implement by user. */
#define EF_INFO(...)  ef_log_info(__VA_ARGS__)
/* EasyFlash assert for developer. */
#define EF_ASSERT(EXPR)                                                       \
if (!(EXPR))                                                                  \
{                                                                             \
    EF_DEBUG("(%s) has assert failed at %s.\n", #EXPR, __FUNCTION__);         \
    while (1);                                                                \
}

/* EasyFlash software version number */
#define EF_SW_VERSION                  "3.1.0"

/* EasyFlash error code */
typedef enum {
    F_NO_ERR,
    F_ERASE_ERR,
    F_READ_ERR,
    F_WRITE_ERR,
    F_DATA_FULL,
    F_DATA_INIT_FAILED,
} EfErrCode;

/* the flash sector current status */
typedef enum {
    EF_SECTOR_EMPTY,
    EF_SECTOR_USING,
    EF_SECTOR_FULL,
}EfSecrorStatus;

/* easyflash.c */
EfErrCode easyflash_init(void);

/* ef_log.c */
EfErrCode ef_log_read(size_t index, uint32_t *log, size_t size);
EfErrCode ef_log_write(const uint32_t *log, size_t size);
EfErrCode ef_log_clean(void);
size_t ef_log_get_used_size(void);


#endif /* EASYFLASH_H_ */
