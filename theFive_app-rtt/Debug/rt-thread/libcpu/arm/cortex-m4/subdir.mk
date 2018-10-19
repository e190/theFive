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
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -x assembler -Xassembler -mimplicit-it=thumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

rt-thread/libcpu/arm/cortex-m4/%.o: ../rt-thread/libcpu/arm/cortex-m4/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -DSTM32F407xx -DHAVE_SYS_SELECT_H -DUSE_HAL_DRIVER -I"D:\Code\eclipse-workspace\theFive_app-rtt-v2.1\applications" -I"D:\Code\eclipse-workspace\theFive_app-rtt-v2.1\components\flash" -I"D:\Code\eclipse-workspace\theFive_app-rtt-v2.1\components\EasyLogger\plugins\flash" -I"D:\Code\eclipse-workspace\theFive_app-rtt-v2.1\rt-thread\components\libc\compilers\newlib" -I"D:\Code\eclipse-workspace\theFive_app-rtt-v2.1\rt-thread\components\libc\compilers\newlib\sys" -I"D:\Code\eclipse-workspace\theFive_app-rtt-v2.1\rt-thread\components\dfs\include" -I"D:\Code\eclipse-workspace\theFive_app-rtt-v2.1\rt-thread\components\dfs\filesystems\devfs" -I"D:\Code\eclipse-workspace\theFive_app-rtt-v2.1\rt-thread\components\dfs\filesystems\elmfat" -I"D:\Code\eclipse-workspace\theFive_app-rtt-v2.1\components\cm_backtrace" -I"D:\Code\eclipse-workspace\theFive_app-rtt-v2.1\rt-thread\components\drivers\include\drivers" -I"D:\Code\eclipse-workspace\theFive_app-rtt-v2.1\rt-thread\packages\cJSON-v1.0.2" -I"D:\Code\eclipse-workspace\theFive_app-rtt-v2.1\components\TMC5130" -I"D:\Code\eclipse-workspace\theFive_app-rtt-v2.1\components\at\include" -I"D:\Code\eclipse-workspace\theFive_app-rtt-v2.1\components\EasyLogger\inc" -I"D:\Code\eclipse-workspace\theFive_app-rtt-v2.1\rt-thread\components\drivers\spi" -I"D:\Code\eclipse-workspace\theFive_app-rtt-v2.1\rt-thread\components\drivers\spi\sfud\inc" -I"D:\Code\eclipse-workspace\theFive_app-rtt-v2.1\rt-thread\components\drivers\include" -I"D:\Code\eclipse-workspace\theFive_app-rtt-v2.1\driver" -I"D:\Code\eclipse-workspace\theFive_app-rtt-v2.1\Libraries\STM32F4xx_HAL_Driver\Inc" -I"D:\Code\eclipse-workspace\theFive_app-rtt-v2.1\Libraries\CMSIS\Include" -I"D:\Code\eclipse-workspace\theFive_app-rtt-v2.1\Libraries\CMSIS\Device\ST\STM32F4xx\Include" -I"D:\Code\eclipse-workspace\theFive_app-rtt-v2.1\rt-thread\include" -I"D:\Code\eclipse-workspace\theFive_app-rtt-v2.1\rt-thread\components\finsh" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


