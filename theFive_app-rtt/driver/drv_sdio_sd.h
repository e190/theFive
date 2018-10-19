#ifndef _SDMMC_SDCARD_H
#define _SDMMC_SDCARD_H
#include <rtthread.h>
#include <rtdevice.h>
#include "string.h"
#include "board.h"

#ifndef SDIO_CLK_DIV
    #define SDIO_CLK_DIV 2
#endif
#define SDIO_TIMEOUT ((uint32_t)0x100000)

#define DEBUG

#ifdef DEBUG
#define DEBUG_PRINTF(...)       rt_kprintf(__VA_ARGS__)
#else
#define DEBUG_PRINTF(...)
#endif


#endif
