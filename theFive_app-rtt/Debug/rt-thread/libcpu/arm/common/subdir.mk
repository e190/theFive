################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../rt-thread/libcpu/arm/common/backtrace.c \
../rt-thread/libcpu/arm/common/div0.c \
../rt-thread/libcpu/arm/common/showmem.c 

OBJS += \
./rt-thread/libcpu/arm/common/backtrace.o \
./rt-thread/libcpu/arm/common/div0.o \
./rt-thread/libcpu/arm/common/showmem.o 

C_DEPS += \
./rt-thread/libcpu/arm/common/backtrace.d \
./rt-thread/libcpu/arm/common/div0.d \
./rt-thread/libcpu/arm/common/showmem.d 


# Each subdirectory must supply rules for building sources it contributes
rt-thread/libcpu/arm/common/%.o: ../rt-thread/libcpu/arm/common/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -DSTM32F407xx -DHAVE_SYS_SELECT_H -DUSE_HAL_DRIVER -I"D:\Code\eclipse-workspace\theFive_app-rtt\applications" -I"D:\Code\eclipse-workspace\theFive_app-rtt\components\ulog\syslog" -I"D:\Code\eclipse-workspace\theFive_app-rtt\components\ulog" -I"D:\Code\eclipse-workspace\theFive_app-rtt\components\flash" -I"D:\Code\eclipse-workspace\theFive_app-rtt\components\EasyLogger\plugins\flash" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\libc\compilers\newlib" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\libc\compilers\newlib\sys" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\dfs\include" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\dfs\filesystems\devfs" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\dfs\filesystems\elmfat" -I"D:\Code\eclipse-workspace\theFive_app-rtt\components\cm_backtrace" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\drivers\include\drivers" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\packages\cJSON-v1.0.2" -I"D:\Code\eclipse-workspace\theFive_app-rtt\components\TMC5130" -I"D:\Code\eclipse-workspace\theFive_app-rtt\components\at\include" -I"D:\Code\eclipse-workspace\theFive_app-rtt\components\EasyLogger\inc" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\drivers\spi" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\drivers\spi\sfud\inc" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\drivers\include" -I"D:\Code\eclipse-workspace\theFive_app-rtt\driver" -I"D:\Code\eclipse-workspace\theFive_app-rtt\Libraries\STM32F4xx_HAL_Driver\Inc" -I"D:\Code\eclipse-workspace\theFive_app-rtt\Libraries\CMSIS\Include" -I"D:\Code\eclipse-workspace\theFive_app-rtt\Libraries\CMSIS\Device\ST\STM32F4xx\Include" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\include" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\finsh" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


