################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../driver/board.c \
../driver/bsp_G780.c \
../driver/bsp_RMD8.c \
../driver/bsp_StepMotor.c \
../driver/bsp_ds18b20.c \
../driver/bsp_rfid.c \
../driver/bsp_timer.c \
../driver/drv_ad1110.c \
../driver/drv_gpio.c \
../driver/drv_i2c.c \
../driver/drv_pwm.c \
../driver/drv_rtc.c \
../driver/drv_sdio_sd.c \
../driver/drv_spi.c \
../driver/drv_spiflash.c \
../driver/drv_usart.c \
../driver/drv_usbd.c \
../driver/stm32f4xx_it.c 

OBJS += \
./driver/board.o \
./driver/bsp_G780.o \
./driver/bsp_RMD8.o \
./driver/bsp_StepMotor.o \
./driver/bsp_ds18b20.o \
./driver/bsp_rfid.o \
./driver/bsp_timer.o \
./driver/drv_ad1110.o \
./driver/drv_gpio.o \
./driver/drv_i2c.o \
./driver/drv_pwm.o \
./driver/drv_rtc.o \
./driver/drv_sdio_sd.o \
./driver/drv_spi.o \
./driver/drv_spiflash.o \
./driver/drv_usart.o \
./driver/drv_usbd.o \
./driver/stm32f4xx_it.o 

C_DEPS += \
./driver/board.d \
./driver/bsp_G780.d \
./driver/bsp_RMD8.d \
./driver/bsp_StepMotor.d \
./driver/bsp_ds18b20.d \
./driver/bsp_rfid.d \
./driver/bsp_timer.d \
./driver/drv_ad1110.d \
./driver/drv_gpio.d \
./driver/drv_i2c.d \
./driver/drv_pwm.d \
./driver/drv_rtc.d \
./driver/drv_sdio_sd.d \
./driver/drv_spi.d \
./driver/drv_spiflash.d \
./driver/drv_usart.d \
./driver/drv_usbd.d \
./driver/stm32f4xx_it.d 


# Each subdirectory must supply rules for building sources it contributes
driver/%.o: ../driver/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -DSTM32F407xx -DHAVE_SYS_SELECT_H -DUSE_HAL_DRIVER -I"D:\Code\eclipse-workspace\theFive_app-rtt\applications" -I"D:\Code\eclipse-workspace\theFive_app-rtt\components\ulog\syslog" -I"D:\Code\eclipse-workspace\theFive_app-rtt\components\ulog" -I"D:\Code\eclipse-workspace\theFive_app-rtt\components\flash" -I"D:\Code\eclipse-workspace\theFive_app-rtt\components\EasyLogger\plugins\flash" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\libc\compilers\newlib" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\libc\compilers\newlib\sys" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\dfs\include" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\dfs\filesystems\devfs" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\dfs\filesystems\elmfat" -I"D:\Code\eclipse-workspace\theFive_app-rtt\components\cm_backtrace" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\drivers\include\drivers" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\packages\cJSON-v1.0.2" -I"D:\Code\eclipse-workspace\theFive_app-rtt\components\TMC5130" -I"D:\Code\eclipse-workspace\theFive_app-rtt\components\at\include" -I"D:\Code\eclipse-workspace\theFive_app-rtt\components\EasyLogger\inc" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\drivers\spi" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\drivers\spi\sfud\inc" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\drivers\include" -I"D:\Code\eclipse-workspace\theFive_app-rtt\driver" -I"D:\Code\eclipse-workspace\theFive_app-rtt\Libraries\STM32F4xx_HAL_Driver\Inc" -I"D:\Code\eclipse-workspace\theFive_app-rtt\Libraries\CMSIS\Include" -I"D:\Code\eclipse-workspace\theFive_app-rtt\Libraries\CMSIS\Device\ST\STM32F4xx\Include" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\include" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\finsh" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


