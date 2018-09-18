################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../rt-thread/libcpu/arm/AT91SAM7S/cpu.c \
../rt-thread/libcpu/arm/AT91SAM7S/interrupt.c \
../rt-thread/libcpu/arm/AT91SAM7S/serial.c \
../rt-thread/libcpu/arm/AT91SAM7S/stack.c \
../rt-thread/libcpu/arm/AT91SAM7S/trap.c 

S_UPPER_SRCS += \
../rt-thread/libcpu/arm/AT91SAM7S/context_gcc.S \
../rt-thread/libcpu/arm/AT91SAM7S/context_rvds.S \
../rt-thread/libcpu/arm/AT91SAM7S/start_gcc.S \
../rt-thread/libcpu/arm/AT91SAM7S/start_rvds.S 

OBJS += \
./rt-thread/libcpu/arm/AT91SAM7S/context_gcc.o \
./rt-thread/libcpu/arm/AT91SAM7S/context_rvds.o \
./rt-thread/libcpu/arm/AT91SAM7S/cpu.o \
./rt-thread/libcpu/arm/AT91SAM7S/interrupt.o \
./rt-thread/libcpu/arm/AT91SAM7S/serial.o \
./rt-thread/libcpu/arm/AT91SAM7S/stack.o \
./rt-thread/libcpu/arm/AT91SAM7S/start_gcc.o \
./rt-thread/libcpu/arm/AT91SAM7S/start_rvds.o \
./rt-thread/libcpu/arm/AT91SAM7S/trap.o 

S_UPPER_DEPS += \
./rt-thread/libcpu/arm/AT91SAM7S/context_gcc.d \
./rt-thread/libcpu/arm/AT91SAM7S/context_rvds.d \
./rt-thread/libcpu/arm/AT91SAM7S/start_gcc.d \
./rt-thread/libcpu/arm/AT91SAM7S/start_rvds.d 

C_DEPS += \
./rt-thread/libcpu/arm/AT91SAM7S/cpu.d \
./rt-thread/libcpu/arm/AT91SAM7S/interrupt.d \
./rt-thread/libcpu/arm/AT91SAM7S/serial.d \
./rt-thread/libcpu/arm/AT91SAM7S/stack.d \
./rt-thread/libcpu/arm/AT91SAM7S/trap.d 


# Each subdirectory must supply rules for building sources it contributes
rt-thread/libcpu/arm/AT91SAM7S/%.o: ../rt-thread/libcpu/arm/AT91SAM7S/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross Assembler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -x assembler-with-cpp -Xassembler -mimplicit-it=thumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

rt-thread/libcpu/arm/AT91SAM7S/%.o: ../rt-thread/libcpu/arm/AT91SAM7S/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -DRT_USING_NEWLIB -DSTM32F407xx -DUSE_HAL_DRIVER -I"D:\Documents\eclipse-workspace\theFive_app-rtt\applications" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\driver" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\Libraries\STM32F4xx_HAL_Driver\Inc" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\Libraries\CMSIS\Include" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\Libraries\CMSIS\Device\ST\STM32F4xx\Include" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\rt-thread\include" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\rt-thread\components\finsh" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


