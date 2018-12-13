################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../rt-thread/libcpu/arm/cortex-m4/cpuport.c 

S_UPPER_SRCS += \
../rt-thread/libcpu/arm/cortex-m4/context_gcc.S 

OBJS += \
./rt-thread/libcpu/arm/cortex-m4/context_gcc.o \
./rt-thread/libcpu/arm/cortex-m4/cpuport.o 

S_UPPER_DEPS += \
./rt-thread/libcpu/arm/cortex-m4/context_gcc.d 

C_DEPS += \
./rt-thread/libcpu/arm/cortex-m4/cpuport.d 


# Each subdirectory must supply rules for building sources it contributes
rt-thread/libcpu/arm/cortex-m4/%.o: ../rt-thread/libcpu/arm/cortex-m4/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross Assembler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

rt-thread/libcpu/arm/cortex-m4/%.o: ../rt-thread/libcpu/arm/cortex-m4/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -DSTM32F407xx -DHAVE_SYS_SELECT_H -DUSE_HAL_DRIVER -I"D:\Code\eclipse-workspace\theFive_app-rtt\Libraries\HAL_Drivers" -I"D:\Code\eclipse-workspace\theFive_app-rtt\Libraries\HAL_Drivers\config" -I"D:\Code\eclipse-workspace\theFive_app-rtt\Libraries\STM32F4xx_HAL\CMSIS\Device\ST\STM32F4xx\Include" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\utilities\ulog\backend" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\utilities\ulog\syslog" -I"D:\Code\eclipse-workspace\theFive_app-rtt\components\cm_backtrace" -I"D:\Code\eclipse-workspace\theFive_app-rtt\components\flash" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\drivers\spi" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\drivers\spi\sfud\inc" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\drivers\usb\usbdevice\class" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\net\at\include" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\net\sal_socket\include\dfs_net" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\net\sal_socket\impl" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\net\at\at_socket" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\utilities\logtrace" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\net\sal_socket\include\socket\sys_socket" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\net\sal_socket\include" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\net\sal_socket\include\socket" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\utilities\ulog" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\utilities\cJSON-v1.0.2" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\utilities\webclient-v2.0.1\inc" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\dfs\include" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\finsh" -I"D:\Code\eclipse-workspace\theFive_app-rtt\Libraries\STM32F4xx_HAL\CMSIS\Include" -I"D:\Code\eclipse-workspace\theFive_app-rtt\Libraries\STM32F4xx_HAL\STM32F4xx_HAL_Driver\Inc" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\include" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\drivers\include" -I"D:\Code\eclipse-workspace\theFive_app-rtt\board" -I"D:\Code\eclipse-workspace\theFive_app-rtt\applications" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


