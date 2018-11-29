#ifndef RT_CONFIG_H__
#define RT_CONFIG_H__

/* Automatically generated file; DO NOT EDIT. */
/* RT-Thread Configuration */

/* RT-Thread Kernel */

#define RT_NAME_MAX 8
#define RT_ALIGN_SIZE 4
#define RT_THREAD_PRIORITY_32
#define RT_THREAD_PRIORITY_MAX 32
#define RT_TICK_PER_SECOND 1000
#define RT_USING_COMPONENTS_INIT
#define RT_USING_USER_MAIN
#define RT_MAIN_THREAD_STACK_SIZE 1024
#define RT_DEBUG
#define RT_DEBUG_INIT 0
#define RT_USING_OVERFLOW_CHECK
#define RT_DEBUG_THREAD 0


#define RT_USING_HOOK
#define IDLE_THREAD_STACK_SIZE 256

#define RT_USING_TIMER_SOFT 1
#if RT_USING_TIMER_SOFT == 0
#undef RT_USING_TIMER_SOFT
#endif
#define RT_TIMER_THREAD_PRIO 4
#define RT_TIMER_THREAD_STACK_SIZE 512
#define RT_TIMER_TICK_PER_SECOND 100

/* Inter-Thread communication */

#define RT_USING_SEMAPHORE
#define RT_USING_MUTEX
#define RT_USING_EVENT
#define RT_USING_MAILBOX
#define RT_USING_MESSAGEQUEUE

/* Memory Management */

#define RT_USING_MEMPOOL
#define RT_USING_MEMHEAP
#define RT_USING_SMALL_MEM
#define RT_USING_HEAP

/* Kernel Device Object */

#define RT_USING_DEVICE
#define RT_USING_CONSOLE
#define RT_CONSOLEBUF_SIZE 384
#define RT_CONSOLE_DEVICE_NAME "uart1"

/* RT-Thread Components */

/* C++ features */

/* RT_USING_CPLUSPLUS is not set */

/* Command shell */

#define RT_USING_FINSH
#define FINSH_THREAD_NAME "tshell"
#define FINSH_USING_HISTORY
#define FINSH_HISTORY_LINES 5
#define FINSH_USING_SYMTAB
#define FINSH_USING_DESCRIPTION
#define FINSH_THREAD_PRIORITY 20
#define FINSH_THREAD_STACK_SIZE 4096
#define FINSH_CMD_SIZE 80
#define FINSH_USING_MSH
#define FINSH_USING_MSH_DEFAULT

/* Device virtual file system */

#define RT_USING_DFS
#define DFS_USING_WORKDIR
#define DFS_FILESYSTEMS_MAX 2
#define DFS_FILESYSTEM_TYPES_MAX 2
#define DFS_FD_MAX 4
#define RT_USING_DFS_ELMFAT

/* elm-chan's FatFs, Generic FAT Filesystem Module */

#define RT_DFS_ELM_CODE_PAGE 437
#define RT_DFS_ELM_WORD_ACCESS
#define RT_DFS_ELM_USE_LFN_3
#define RT_DFS_ELM_USE_LFN 3
#define RT_DFS_ELM_MAX_LFN 255
#define RT_DFS_ELM_DRIVES 2
#define RT_DFS_ELM_MAX_SECTOR_SIZE 4096
#define RT_DFS_ELM_REENTRANT
#define RT_USING_DFS_DEVFS

/* Device Drivers */

#define RT_USING_DEVICE_IPC
#define RT_USING_SERIAL

#define RT_USING_I2C
#define RT_USING_I2C_BITOPS
#define RT_USING_I2C_1
#define RT_USING_I2C_2
#define RT_USING_I2C_3
#define RT_USING_I2C_4
#define RT_USING_I2C_5
//#define RT_I2C_BIT_DEBUG
#define RT_USING_PIN
#define RT_USING_PWM
#define RT_USING_RTC

#define RT_USING_SPI
//#define RT_USING_W25QXX
#define RT_USING_SFUD
#define RT_SFUD_USING_SFDP
#define RT_SFUD_USING_FLASH_INFO_TABLE
//#define RT_DEBUG_SFUD  0

#define RT_USING_CPUTIME

/* Using USB */
#define RT_USING_USB_DEVICE
#define RT_USBD_THREAD_STACK_SZ 4096
#define USB_VENDOR_ID 0x0483
#define USB_PRODUCT_ID 0x5710
#define RT_USB_DEVICE_COMPOSITE
#define RT_USB_DEVICE_NONE
#define RT_USB_DEVICE_MSTORAGE
#define RT_USB_MSTORAGE_DISK_NAME 	"sd0"
#define RT_USB_DEVICE_HID
#define RT_USB_DEVICE_HID_GENERAL
#define RT_USB_DEVICE_HID_GENERAL_OUT_REPORT_LENGTH 63
#define RT_USB_DEVICE_HID_GENERAL_IN_REPORT_LENGTH 63

/* POSIX layer and C standard library */
#define RT_USING_LIBC
#define RT_USING_POSIX
#define RT_USING_NEWLIB

/* AT commands */

#define RT_USING_AT
#define AT_DEBUG
#define AT_USING_CLIENT
#define AT_CLIENT_DEVICE "uart4"
#define AT_CLIENT_RECV_BUFF_LEN 512
#define AT_USING_SOCKET
#define AT_USING_CLI
//#define AT_PRINT_RAW_CMD

/* RT-Thread online packages */

/* system packages */

#define RT_USING_ULOG
#define ULOG_OUTPUT_LVL_DEBUG
#define ULOG_OUTPUT_LVL 7
#define ULOG_ASSERT_ENABLE
#define ULOG_LINE_BUF_SIZE 128

/* log format */

#define ULOG_OUTPUT_FLOAT
#define ULOG_USING_COLOR
#define ULOG_OUTPUT_TIME
#define ULOG_OUTPUT_LEVEL
#define ULOG_OUTPUT_TAG
#define ULOG_BACKEND_USING_CONSOLE
//#define ULOG_BACKEND_USING_FLASH
#define ULOG_SW_VERSION_NUM 0x00100

/* IoT - internet of things */

#define PKG_USING_CJSON
#define PKG_USING_CJSON_V102

/* miscellaneous packages */

#define RT_HSE_VALUE 8000000
#define RT_HSE_HCLK 168000000

#endif
