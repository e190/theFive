################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../rt-thread/libcpu/arm/sep4020/clk.c \
../rt-thread/libcpu/arm/sep4020/cpu.c \
../rt-thread/libcpu/arm/sep4020/interrupt.c \
../rt-thread/libcpu/arm/sep4020/serial.c \
../rt-thread/libcpu/arm/sep4020/stack.c \
../rt-thread/libcpu/arm/sep4020/trap.c 

S_UPPER_SRCS += \
../rt-thread/libcpu/arm/sep4020/context_rvds.S \
../rt-thread/libcpu/arm/sep4020/start_rvds.S 

OBJS += \
./rt-thread/libcpu/arm/sep4020/clk.o \
./rt-thread/libcpu/arm/sep4020/context_rvds.o \
./rt-thread/libcpu/arm/sep4020/cpu.o \
./rt-thread/libcpu/arm/sep4020/interrupt.o \
./rt-thread/libcpu/arm/sep4020/serial.o \
./rt-thread/libcpu/arm/sep4020/stack.o \
./rt-thread/libcpu/arm/sep4020/start_rvds.o \
./rt-thread/libcpu/arm/sep4020/trap.o 

S_UPPER_DEPS += \
./rt-thread/libcpu/arm/sep4020/context_rvds.d \
./rt-thread/libcpu/arm/sep4020/start_rvds.d 

C_DEPS += \
./rt-thread/libcpu/arm/sep4020/clk.d \
./rt-thread/libcpu/arm/sep4020/cpu.d \
./rt-thread/libcpu/arm/sep4020/interrupt.d \
./rt-thread/libcpu/arm/sep4020/serial.d \
./rt-thread/libcpu/arm/sep4020/stack.d \
./rt-thread/libcpu/arm/sep4020/trap.d 


# Each subdirectory must supply rules for building sources it contributes
rt-thread/libcpu/arm/sep4020/%.o: ../rt-thread/libcpu/arm/sep4020/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -DRT_USING_NEWLIB -DSTM32F407xx -DUSE_HAL_DRIVER -I"D:\Documents\eclipse-workspace\theFive_app-rtt\applications" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\driver" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\Libraries\STM32F4xx_HAL_Driver\Inc" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\Libraries\CMSIS\Include" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\Libraries\CMSIS\Device\ST\STM32F4xx\Include" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\rt-thread\include" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\rt-thread\components\finsh" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

rt-thread/libcpu/arm/sep4020/%.o: ../rt-thread/libcpu/arm/sep4020/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross Assembler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -x assembler-with-cpp -Xassembler -mimplicit-it=thumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


