################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../rt-thread/libcpu/arm/realview-a8-vmm/cpu.c \
../rt-thread/libcpu/arm/realview-a8-vmm/gic.c \
../rt-thread/libcpu/arm/realview-a8-vmm/interrupt.c \
../rt-thread/libcpu/arm/realview-a8-vmm/mmu.c \
../rt-thread/libcpu/arm/realview-a8-vmm/pmu.c \
../rt-thread/libcpu/arm/realview-a8-vmm/stack.c \
../rt-thread/libcpu/arm/realview-a8-vmm/trap.c 

S_UPPER_SRCS += \
../rt-thread/libcpu/arm/realview-a8-vmm/context_gcc.S \
../rt-thread/libcpu/arm/realview-a8-vmm/cp15_gcc.S \
../rt-thread/libcpu/arm/realview-a8-vmm/start_gcc.S \
../rt-thread/libcpu/arm/realview-a8-vmm/vector_gcc.S 

OBJS += \
./rt-thread/libcpu/arm/realview-a8-vmm/context_gcc.o \
./rt-thread/libcpu/arm/realview-a8-vmm/cp15_gcc.o \
./rt-thread/libcpu/arm/realview-a8-vmm/cpu.o \
./rt-thread/libcpu/arm/realview-a8-vmm/gic.o \
./rt-thread/libcpu/arm/realview-a8-vmm/interrupt.o \
./rt-thread/libcpu/arm/realview-a8-vmm/mmu.o \
./rt-thread/libcpu/arm/realview-a8-vmm/pmu.o \
./rt-thread/libcpu/arm/realview-a8-vmm/stack.o \
./rt-thread/libcpu/arm/realview-a8-vmm/start_gcc.o \
./rt-thread/libcpu/arm/realview-a8-vmm/trap.o \
./rt-thread/libcpu/arm/realview-a8-vmm/vector_gcc.o 

S_UPPER_DEPS += \
./rt-thread/libcpu/arm/realview-a8-vmm/context_gcc.d \
./rt-thread/libcpu/arm/realview-a8-vmm/cp15_gcc.d \
./rt-thread/libcpu/arm/realview-a8-vmm/start_gcc.d \
./rt-thread/libcpu/arm/realview-a8-vmm/vector_gcc.d 

C_DEPS += \
./rt-thread/libcpu/arm/realview-a8-vmm/cpu.d \
./rt-thread/libcpu/arm/realview-a8-vmm/gic.d \
./rt-thread/libcpu/arm/realview-a8-vmm/interrupt.d \
./rt-thread/libcpu/arm/realview-a8-vmm/mmu.d \
./rt-thread/libcpu/arm/realview-a8-vmm/pmu.d \
./rt-thread/libcpu/arm/realview-a8-vmm/stack.d \
./rt-thread/libcpu/arm/realview-a8-vmm/trap.d 


# Each subdirectory must supply rules for building sources it contributes
rt-thread/libcpu/arm/realview-a8-vmm/%.o: ../rt-thread/libcpu/arm/realview-a8-vmm/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross Assembler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -x assembler-with-cpp -Xassembler -mimplicit-it=thumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

rt-thread/libcpu/arm/realview-a8-vmm/%.o: ../rt-thread/libcpu/arm/realview-a8-vmm/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -DRT_USING_NEWLIB -DSTM32F407xx -DUSE_HAL_DRIVER -I"D:\Documents\eclipse-workspace\theFive_app-rtt\applications" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\driver" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\Libraries\STM32F4xx_HAL_Driver\Inc" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\Libraries\CMSIS\Include" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\Libraries\CMSIS\Device\ST\STM32F4xx\Include" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\rt-thread\include" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\rt-thread\components\finsh" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


