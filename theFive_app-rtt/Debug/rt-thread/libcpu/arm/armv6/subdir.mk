################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../rt-thread/libcpu/arm/armv6/cpuport.c \
../rt-thread/libcpu/arm/armv6/mmu.c \
../rt-thread/libcpu/arm/armv6/stack.c \
../rt-thread/libcpu/arm/armv6/vfp.c 

S_UPPER_SRCS += \
../rt-thread/libcpu/arm/armv6/arm_entry_gcc.S \
../rt-thread/libcpu/arm/armv6/context_gcc.S 

OBJS += \
./rt-thread/libcpu/arm/armv6/arm_entry_gcc.o \
./rt-thread/libcpu/arm/armv6/context_gcc.o \
./rt-thread/libcpu/arm/armv6/cpuport.o \
./rt-thread/libcpu/arm/armv6/mmu.o \
./rt-thread/libcpu/arm/armv6/stack.o \
./rt-thread/libcpu/arm/armv6/vfp.o 

S_UPPER_DEPS += \
./rt-thread/libcpu/arm/armv6/arm_entry_gcc.d \
./rt-thread/libcpu/arm/armv6/context_gcc.d 

C_DEPS += \
./rt-thread/libcpu/arm/armv6/cpuport.d \
./rt-thread/libcpu/arm/armv6/mmu.d \
./rt-thread/libcpu/arm/armv6/stack.d \
./rt-thread/libcpu/arm/armv6/vfp.d 


# Each subdirectory must supply rules for building sources it contributes
rt-thread/libcpu/arm/armv6/%.o: ../rt-thread/libcpu/arm/armv6/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross Assembler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -x assembler-with-cpp -Xassembler -mimplicit-it=thumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

rt-thread/libcpu/arm/armv6/%.o: ../rt-thread/libcpu/arm/armv6/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -DRT_USING_NEWLIB -DSTM32F407xx -DUSE_HAL_DRIVER -I"D:\Documents\eclipse-workspace\theFive_app-rtt\applications" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\driver" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\Libraries\STM32F4xx_HAL_Driver\Inc" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\Libraries\CMSIS\Include" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\Libraries\CMSIS\Device\ST\STM32F4xx\Include" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\rt-thread\include" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\rt-thread\components\finsh" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


