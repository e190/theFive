################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../rt-thread/libcpu/mips/xburst/cache.c \
../rt-thread/libcpu/mips/xburst/cpu.c \
../rt-thread/libcpu/mips/xburst/exception.c \
../rt-thread/libcpu/mips/xburst/interrupt.c \
../rt-thread/libcpu/mips/xburst/mipscfg.c \
../rt-thread/libcpu/mips/xburst/stack.c 

S_UPPER_SRCS += \
../rt-thread/libcpu/mips/xburst/cache_gcc.S \
../rt-thread/libcpu/mips/xburst/context_gcc.S \
../rt-thread/libcpu/mips/xburst/start_gcc.S 

OBJS += \
./rt-thread/libcpu/mips/xburst/cache.o \
./rt-thread/libcpu/mips/xburst/cache_gcc.o \
./rt-thread/libcpu/mips/xburst/context_gcc.o \
./rt-thread/libcpu/mips/xburst/cpu.o \
./rt-thread/libcpu/mips/xburst/exception.o \
./rt-thread/libcpu/mips/xburst/interrupt.o \
./rt-thread/libcpu/mips/xburst/mipscfg.o \
./rt-thread/libcpu/mips/xburst/stack.o \
./rt-thread/libcpu/mips/xburst/start_gcc.o 

S_UPPER_DEPS += \
./rt-thread/libcpu/mips/xburst/cache_gcc.d \
./rt-thread/libcpu/mips/xburst/context_gcc.d \
./rt-thread/libcpu/mips/xburst/start_gcc.d 

C_DEPS += \
./rt-thread/libcpu/mips/xburst/cache.d \
./rt-thread/libcpu/mips/xburst/cpu.d \
./rt-thread/libcpu/mips/xburst/exception.d \
./rt-thread/libcpu/mips/xburst/interrupt.d \
./rt-thread/libcpu/mips/xburst/mipscfg.d \
./rt-thread/libcpu/mips/xburst/stack.d 


# Each subdirectory must supply rules for building sources it contributes
rt-thread/libcpu/mips/xburst/%.o: ../rt-thread/libcpu/mips/xburst/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -DRT_USING_NEWLIB -DSTM32F407xx -DUSE_HAL_DRIVER -I"D:\Documents\eclipse-workspace\theFive_app-rtt\applications" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\driver" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\Libraries\STM32F4xx_HAL_Driver\Inc" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\Libraries\CMSIS\Include" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\Libraries\CMSIS\Device\ST\STM32F4xx\Include" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\rt-thread\include" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\rt-thread\components\finsh" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

rt-thread/libcpu/mips/xburst/%.o: ../rt-thread/libcpu/mips/xburst/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross Assembler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -x assembler-with-cpp -Xassembler -mimplicit-it=thumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


