################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../rt-thread/libcpu/ia32/__udivsi3.c \
../rt-thread/libcpu/ia32/__umodsi3.c \
../rt-thread/libcpu/ia32/backtrace.c \
../rt-thread/libcpu/ia32/interrupt.c \
../rt-thread/libcpu/ia32/showmem.c \
../rt-thread/libcpu/ia32/stack.c \
../rt-thread/libcpu/ia32/trap.c 

S_UPPER_SRCS += \
../rt-thread/libcpu/ia32/context_gcc.S \
../rt-thread/libcpu/ia32/hdisr_gcc.S \
../rt-thread/libcpu/ia32/start_gcc.S \
../rt-thread/libcpu/ia32/trapisr_gcc.S 

OBJS += \
./rt-thread/libcpu/ia32/__udivsi3.o \
./rt-thread/libcpu/ia32/__umodsi3.o \
./rt-thread/libcpu/ia32/backtrace.o \
./rt-thread/libcpu/ia32/context_gcc.o \
./rt-thread/libcpu/ia32/hdisr_gcc.o \
./rt-thread/libcpu/ia32/interrupt.o \
./rt-thread/libcpu/ia32/showmem.o \
./rt-thread/libcpu/ia32/stack.o \
./rt-thread/libcpu/ia32/start_gcc.o \
./rt-thread/libcpu/ia32/trap.o \
./rt-thread/libcpu/ia32/trapisr_gcc.o 

S_UPPER_DEPS += \
./rt-thread/libcpu/ia32/context_gcc.d \
./rt-thread/libcpu/ia32/hdisr_gcc.d \
./rt-thread/libcpu/ia32/start_gcc.d \
./rt-thread/libcpu/ia32/trapisr_gcc.d 

C_DEPS += \
./rt-thread/libcpu/ia32/__udivsi3.d \
./rt-thread/libcpu/ia32/__umodsi3.d \
./rt-thread/libcpu/ia32/backtrace.d \
./rt-thread/libcpu/ia32/interrupt.d \
./rt-thread/libcpu/ia32/showmem.d \
./rt-thread/libcpu/ia32/stack.d \
./rt-thread/libcpu/ia32/trap.d 


# Each subdirectory must supply rules for building sources it contributes
rt-thread/libcpu/ia32/%.o: ../rt-thread/libcpu/ia32/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -DRT_USING_NEWLIB -DSTM32F407xx -DUSE_HAL_DRIVER -I"D:\Documents\eclipse-workspace\theFive_app-rtt\applications" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\driver" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\Libraries\STM32F4xx_HAL_Driver\Inc" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\Libraries\CMSIS\Include" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\Libraries\CMSIS\Device\ST\STM32F4xx\Include" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\rt-thread\include" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\rt-thread\components\finsh" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

rt-thread/libcpu/ia32/%.o: ../rt-thread/libcpu/ia32/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross Assembler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -x assembler-with-cpp -Xassembler -mimplicit-it=thumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


