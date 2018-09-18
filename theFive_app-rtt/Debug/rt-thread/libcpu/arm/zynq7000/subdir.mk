################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../rt-thread/libcpu/arm/zynq7000/cpu.c \
../rt-thread/libcpu/arm/zynq7000/gic.c \
../rt-thread/libcpu/arm/zynq7000/interrupt.c \
../rt-thread/libcpu/arm/zynq7000/mmu.c \
../rt-thread/libcpu/arm/zynq7000/stack.c \
../rt-thread/libcpu/arm/zynq7000/trap.c 

S_UPPER_SRCS += \
../rt-thread/libcpu/arm/zynq7000/context_gcc.S \
../rt-thread/libcpu/arm/zynq7000/cp15_gcc.S \
../rt-thread/libcpu/arm/zynq7000/start_gcc.S \
../rt-thread/libcpu/arm/zynq7000/vector_gcc.S 

OBJS += \
./rt-thread/libcpu/arm/zynq7000/context_gcc.o \
./rt-thread/libcpu/arm/zynq7000/cp15_gcc.o \
./rt-thread/libcpu/arm/zynq7000/cpu.o \
./rt-thread/libcpu/arm/zynq7000/gic.o \
./rt-thread/libcpu/arm/zynq7000/interrupt.o \
./rt-thread/libcpu/arm/zynq7000/mmu.o \
./rt-thread/libcpu/arm/zynq7000/stack.o \
./rt-thread/libcpu/arm/zynq7000/start_gcc.o \
./rt-thread/libcpu/arm/zynq7000/trap.o \
./rt-thread/libcpu/arm/zynq7000/vector_gcc.o 

S_UPPER_DEPS += \
./rt-thread/libcpu/arm/zynq7000/context_gcc.d \
./rt-thread/libcpu/arm/zynq7000/cp15_gcc.d \
./rt-thread/libcpu/arm/zynq7000/start_gcc.d \
./rt-thread/libcpu/arm/zynq7000/vector_gcc.d 

C_DEPS += \
./rt-thread/libcpu/arm/zynq7000/cpu.d \
./rt-thread/libcpu/arm/zynq7000/gic.d \
./rt-thread/libcpu/arm/zynq7000/interrupt.d \
./rt-thread/libcpu/arm/zynq7000/mmu.d \
./rt-thread/libcpu/arm/zynq7000/stack.d \
./rt-thread/libcpu/arm/zynq7000/trap.d 


# Each subdirectory must supply rules for building sources it contributes
rt-thread/libcpu/arm/zynq7000/%.o: ../rt-thread/libcpu/arm/zynq7000/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross Assembler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -x assembler-with-cpp -Xassembler -mimplicit-it=thumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

rt-thread/libcpu/arm/zynq7000/%.o: ../rt-thread/libcpu/arm/zynq7000/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -DRT_USING_NEWLIB -DSTM32F407xx -DUSE_HAL_DRIVER -I"D:\Documents\eclipse-workspace\theFive_app-rtt\applications" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\driver" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\Libraries\STM32F4xx_HAL_Driver\Inc" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\Libraries\CMSIS\Include" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\Libraries\CMSIS\Device\ST\STM32F4xx\Include" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\rt-thread\include" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\rt-thread\components\finsh" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


