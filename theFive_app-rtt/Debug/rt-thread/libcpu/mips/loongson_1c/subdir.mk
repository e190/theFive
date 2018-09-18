################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../rt-thread/libcpu/mips/loongson_1c/cache.c \
../rt-thread/libcpu/mips/loongson_1c/cpuport.c \
../rt-thread/libcpu/mips/loongson_1c/exception.c \
../rt-thread/libcpu/mips/loongson_1c/interrupt.c \
../rt-thread/libcpu/mips/loongson_1c/mipscfg.c 

S_UPPER_SRCS += \
../rt-thread/libcpu/mips/loongson_1c/cache_gcc.S \
../rt-thread/libcpu/mips/loongson_1c/context_gcc.S \
../rt-thread/libcpu/mips/loongson_1c/start_gcc.S 

OBJS += \
./rt-thread/libcpu/mips/loongson_1c/cache.o \
./rt-thread/libcpu/mips/loongson_1c/cache_gcc.o \
./rt-thread/libcpu/mips/loongson_1c/context_gcc.o \
./rt-thread/libcpu/mips/loongson_1c/cpuport.o \
./rt-thread/libcpu/mips/loongson_1c/exception.o \
./rt-thread/libcpu/mips/loongson_1c/interrupt.o \
./rt-thread/libcpu/mips/loongson_1c/mipscfg.o \
./rt-thread/libcpu/mips/loongson_1c/start_gcc.o 

S_UPPER_DEPS += \
./rt-thread/libcpu/mips/loongson_1c/cache_gcc.d \
./rt-thread/libcpu/mips/loongson_1c/context_gcc.d \
./rt-thread/libcpu/mips/loongson_1c/start_gcc.d 

C_DEPS += \
./rt-thread/libcpu/mips/loongson_1c/cache.d \
./rt-thread/libcpu/mips/loongson_1c/cpuport.d \
./rt-thread/libcpu/mips/loongson_1c/exception.d \
./rt-thread/libcpu/mips/loongson_1c/interrupt.d \
./rt-thread/libcpu/mips/loongson_1c/mipscfg.d 


# Each subdirectory must supply rules for building sources it contributes
rt-thread/libcpu/mips/loongson_1c/%.o: ../rt-thread/libcpu/mips/loongson_1c/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -DRT_USING_NEWLIB -DSTM32F407xx -DUSE_HAL_DRIVER -I"D:\Documents\eclipse-workspace\theFive_app-rtt\applications" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\driver" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\Libraries\STM32F4xx_HAL_Driver\Inc" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\Libraries\CMSIS\Include" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\Libraries\CMSIS\Device\ST\STM32F4xx\Include" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\rt-thread\include" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\rt-thread\components\finsh" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

rt-thread/libcpu/mips/loongson_1c/%.o: ../rt-thread/libcpu/mips/loongson_1c/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross Assembler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -x assembler-with-cpp -Xassembler -mimplicit-it=thumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


