################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../components/EasyFlash-v3.1.0/src/easyflash.c \
../components/EasyFlash-v3.1.0/src/ef_cmd.c \
../components/EasyFlash-v3.1.0/src/ef_env.c \
../components/EasyFlash-v3.1.0/src/ef_env_wl.c \
../components/EasyFlash-v3.1.0/src/ef_iap.c \
../components/EasyFlash-v3.1.0/src/ef_log.c \
../components/EasyFlash-v3.1.0/src/ef_utils.c 

OBJS += \
./components/EasyFlash-v3.1.0/src/easyflash.o \
./components/EasyFlash-v3.1.0/src/ef_cmd.o \
./components/EasyFlash-v3.1.0/src/ef_env.o \
./components/EasyFlash-v3.1.0/src/ef_env_wl.o \
./components/EasyFlash-v3.1.0/src/ef_iap.o \
./components/EasyFlash-v3.1.0/src/ef_log.o \
./components/EasyFlash-v3.1.0/src/ef_utils.o 

C_DEPS += \
./components/EasyFlash-v3.1.0/src/easyflash.d \
./components/EasyFlash-v3.1.0/src/ef_cmd.d \
./components/EasyFlash-v3.1.0/src/ef_env.d \
./components/EasyFlash-v3.1.0/src/ef_env_wl.d \
./components/EasyFlash-v3.1.0/src/ef_iap.d \
./components/EasyFlash-v3.1.0/src/ef_log.d \
./components/EasyFlash-v3.1.0/src/ef_utils.d 


# Each subdirectory must supply rules for building sources it contributes
components/EasyFlash-v3.1.0/src/%.o: ../components/EasyFlash-v3.1.0/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -DSTM32F407xx -DHAVE_SYS_SELECT_H -DUSE_HAL_DRIVER -I"D:\Code\eclipse-workspace\theFive_app-rtt\applications" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\libc\compilers\newlib" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\libc\compilers\newlib\sys" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\dfs\include" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\dfs\filesystems\devfs" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\dfs\filesystems\elmfat" -I"D:\Code\eclipse-workspace\theFive_app-rtt\components\cm_backtrace" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\drivers\include\drivers" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\packages\cJSON-v1.0.2" -I"D:\Code\eclipse-workspace\theFive_app-rtt\components\TMC5130" -I"D:\Code\eclipse-workspace\theFive_app-rtt\components\at\include" -I"D:\Code\eclipse-workspace\theFive_app-rtt\components\EasyFlash-v3.1.0\inc" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\drivers\spi" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\drivers\spi\sfud\inc" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\drivers\include" -I"D:\Code\eclipse-workspace\theFive_app-rtt\driver" -I"D:\Code\eclipse-workspace\theFive_app-rtt\Libraries\STM32F4xx_HAL_Driver\Inc" -I"D:\Code\eclipse-workspace\theFive_app-rtt\Libraries\CMSIS\Include" -I"D:\Code\eclipse-workspace\theFive_app-rtt\Libraries\CMSIS\Device\ST\STM32F4xx\Include" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\include" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\finsh" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


