################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../rt-thread/components/drivers/cputime/cputime.c \
../rt-thread/components/drivers/cputime/cputime_cortexm.c 

OBJS += \
./rt-thread/components/drivers/cputime/cputime.o \
./rt-thread/components/drivers/cputime/cputime_cortexm.o 

C_DEPS += \
./rt-thread/components/drivers/cputime/cputime.d \
./rt-thread/components/drivers/cputime/cputime_cortexm.d 


# Each subdirectory must supply rules for building sources it contributes
rt-thread/components/drivers/cputime/%.o: ../rt-thread/components/drivers/cputime/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -DSTM32F407xx -DHAVE_SYS_SELECT_H -DUSE_HAL_DRIVER -I"D:\Code\eclipse-workspace\theFive_app-rtt\Libraries\HAL_Drivers" -I"D:\Code\eclipse-workspace\theFive_app-rtt\components\fal\inc" -I"D:\Code\eclipse-workspace\theFive_app-rtt\Libraries\HAL_Drivers\config" -I"D:\Code\eclipse-workspace\theFive_app-rtt\Libraries\STM32F4xx_HAL\CMSIS\Device\ST\STM32F4xx\Include" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\utilities\ulog\backend" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\utilities\ulog\syslog" -I"D:\Code\eclipse-workspace\theFive_app-rtt\components\cm_backtrace" -I"D:\Code\eclipse-workspace\theFive_app-rtt\components\flash" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\drivers\spi" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\drivers\spi\sfud\inc" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\drivers\usb\usbdevice\class" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\net\at\include" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\net\sal_socket\include\dfs_net" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\net\sal_socket\impl" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\net\at\at_socket" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\utilities\logtrace" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\net\sal_socket\include\socket\sys_socket" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\net\sal_socket\include" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\net\sal_socket\include\socket" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\utilities\ulog" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\utilities\cJSON-v1.0.2" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\utilities\webclient-v2.0.1\inc" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\dfs\include" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\finsh" -I"D:\Code\eclipse-workspace\theFive_app-rtt\Libraries\STM32F4xx_HAL\CMSIS\Include" -I"D:\Code\eclipse-workspace\theFive_app-rtt\Libraries\STM32F4xx_HAL\STM32F4xx_HAL_Driver\Inc" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\include" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\drivers\include" -I"D:\Code\eclipse-workspace\theFive_app-rtt\board" -I"D:\Code\eclipse-workspace\theFive_app-rtt\applications" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


