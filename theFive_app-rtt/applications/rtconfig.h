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
#define FINSH_USING_MSH
#define FINSH_USING_MSH_DEFAULT
#define FINSH_USING_MSH_ONLY
#define FINSH_THREAD_NAME "tshell"
#define FINSH_USING_HISTORY
#define FINSH_THREAD_PRIORITY 20
#define FINSH_THREAD_STACK_SIZE 4096
#define FINSH_CMD_SIZE 80
//#define FINSH_USING_MSH
//#define FINSH_USING_MSH_DEFAULT
#define FINSH_USING_SYMTAB
#define FINSH_USING_DESCRIPTION

/* Device virtual file system */

/* RT_USING_DFS is not set */

/* Device Drivers */

#define RT_USING_DEVICE_IPC
#define RT_USING_SERIAL
/* RT_USING_CAN is not set */
/* RT_USING_HWTIMER is not set */
#define RT_USING_I2C
#define RT_USING_I2C_BITOPS
#define RT_USING_I2C_1
#define RT_USING_I2C_2
#define RT_USING_I2C_5
//#define RT_I2C_BIT_DEBUG
#define RT_USING_PIN
#define RT_USING_PWM
/* RT_USING_MTD_NOR is not set */
/* RT_USING_MTD_NAND is not set */
/* RT_USING_RTC is not set */
/* RT_USING_SDIO is not set */
#define RT_USING_SPI
//#define RT_USING_W25QXX
#define RT_USING_SFUD
#define RT_SFUD_USING_SFDP
#define RT_SFUD_USING_FLASH_INFO_TABLE
/* RT_USING_WDT is not set */
/* RT_USING_WIFI is not set */

/* Using USB */
#define RT_USING_USB_DEVICE
#define RT_USBD_THREAD_STACK_SZ 4096
#define USB_VENDOR_ID 0x0483
#define USB_PRODUCT_ID 0x5710
#define _RT_USB_DEVICE_HID
#define RT_USB_DEVICE_HID
#define RT_USB_DEVICE_HID_GENERAL
#define RT_USB_DEVICE_HID_GENERAL_OUT_REPORT_LENGTH 63
#define RT_USB_DEVICE_HID_GENERAL_IN_REPORT_LENGTH 63
#define RT_USB_DEVICE_HID_MEDIA
/* RT_USING_USB_HOST is not set */
/* RT_USING_USB_DEVICE is not set */

/* POSIX layer and C standard library */
#define RT_USING_LIBC
/* RT_USING_LIBC is not set */
/* RT_USING_PTHREADS is not set */
/* Network stack */

/* light weight TCP/IP stack */

/* RT_USING_LWIP is not set */

/* Modbus master and slave stack */

/* RT_USING_MODBUS is not set */

/* RT-Thread UI Engine */

/* PKG_USING_GUIENGINE is not set */
/* AT commands */

#define RT_USING_AT
#define AT_DEBUG
#define AT_USING_CLIENT
#define AT_CLIENT_DEVICE "uart4"
#define AT_CLIENT_RECV_BUFF_LEN 512
#define AT_USING_SOCKET
#define AT_USING_CLI
//#define AT_PRINT_RAW_CMD
/* VBUS(Virtual Software BUS) */

/* RT_USING_VBUS is not set */

/* Utilities */

/* RT_USING_LOGTRACE is not set */
/* RT_USING_RYM is not set */

/* RT-Thread online packages */

/* system packages */

/* PKG_USING_PARTITION is not set */
/* PKG_USING_SQLITE is not set */

/* IoT - internet of things */

#define PKG_USING_CJSON
#define PKG_USING_CJSON_V102
/* PKG_USING_PAHOMQTT is not set */
/* PKG_USING_WEBCLIENT is not set */
/* PKG_USING_MONGOOSE is not set */
/* PKG_USING_WEBTERMINAL is not set */

/* security packages */

/* PKG_USING_MBEDTLS is not set */

/* language packages */

/* PKG_USING_JERRYSCRIPT is not set */

/* multimedia packages */
#define PKG_USING_SAMPLES
/* tools packages */

/* PKG_USING_CMBACKTRACE is not set */
/* PKG_USING_EASYLOGGER is not set */

/* miscellaneous packages */

#define SOC_STM32F407ZE

/* RT_USING_HSI is not set */
#define RT_HSE_VALUE 8000000
#define RT_HSE_HCLK 168000000
/* RT_USING_UART1 is not set */
#define RT_USING_UART3
#define RT_USING_UART2
#define RT_USING_UART4
#define RT_USING_UART1
/* RT_USING_UART6 is not set */
#define BSP_USING_PWM1
#define BSP_USING_PWM1_CH1
#define BSP_USING_PWM1_CH2
#define BSP_USING_PWM1_CH3
#define BSP_USING_PWM1_CH4
#define BSP_USING_PWM2
#define BSP_USING_PWM2_CH1
#define BSP_USING_PWM2_CH2
//#define BSP_USING_PWM3
//#define BSP_USING_PWM3_CH3
//#define BSP_USING_PWM3_CH4
#define RT_USING_SPI2
#define RT_USING_SPI3
//#define RT_DEBUG_SFUD  1
// <<< end of configuration section >>> 
#endif
