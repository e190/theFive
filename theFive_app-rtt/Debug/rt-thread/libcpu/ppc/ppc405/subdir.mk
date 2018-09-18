################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../rt-thread/libcpu/ppc/ppc405/interrupt.c \
../rt-thread/libcpu/ppc/ppc405/serial.c \
../rt-thread/libcpu/ppc/ppc405/traps.c 

S_UPPER_SRCS += \
../rt-thread/libcpu/ppc/ppc405/cache_gcc.S \
../rt-thread/libcpu/ppc/ppc405/context_gcc.S \
../rt-thread/libcpu/ppc/ppc405/dcr_gcc.S \
../rt-thread/libcpu/ppc/ppc405/start_gcc.S 

OBJS += \
./rt-thread/libcpu/ppc/ppc405/cache_gcc.o \
./rt-thread/libcpu/ppc/ppc405/context_gcc.o \
./rt-thread/libcpu/ppc/ppc405/dcr_gcc.o \
./rt-thread/libcpu/ppc/ppc405/interrupt.o \
./rt-thread/libcpu/ppc/ppc405/serial.o \
./rt-thread/libcpu/ppc/ppc405/start_gcc.o \
./rt-thread/libcpu/ppc/ppc405/traps.o 

S_UPPER_DEPS += \
./rt-thread/libcpu/ppc/ppc405/cache_gcc.d \
./rt-thread/libcpu/ppc/ppc405/context_gcc.d \
./rt-thread/libcpu/ppc/ppc405/dcr_gcc.d \
./rt-thread/libcpu/ppc/ppc405/start_gcc.d 

C_DEPS += \
./rt-thread/libcpu/ppc/ppc405/interrupt.d \
./rt-thread/libcpu/ppc/ppc405/serial.d \
./rt-thread/libcpu/ppc/ppc405/traps.d 


# Each subdirectory must supply rules for building sources it contributes
rt-thread/libcpu/ppc/ppc405/%.o: ../rt-thread/libcpu/ppc/ppc405/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross Assembler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -x assembler-with-cpp -Xassembler -mimplicit-it=thumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

rt-thread/libcpu/ppc/ppc405/%.o: ../rt-thread/libcpu/ppc/ppc405/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -DRT_USING_NEWLIB -DSTM32F407xx -DUSE_HAL_DRIVER -I"D:\Documents\eclipse-workspace\theFive_app-rtt\applications" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\driver" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\Libraries\STM32F4xx_HAL_Driver\Inc" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\Libraries\CMSIS\Include" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\Libraries\CMSIS\Device\ST\STM32F4xx\Include" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\rt-thread\include" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\rt-thread\components\finsh" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


