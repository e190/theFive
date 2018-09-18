################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../rt-thread/libcpu/mips/x1000/cache.c \
../rt-thread/libcpu/mips/x1000/cpu.c \
../rt-thread/libcpu/mips/x1000/interrupt.c \
../rt-thread/libcpu/mips/x1000/mips_backtrace.c \
../rt-thread/libcpu/mips/x1000/mips_excpt.c \
../rt-thread/libcpu/mips/x1000/stack.c 

S_UPPER_SRCS += \
../rt-thread/libcpu/mips/x1000/mips_cache_gcc.S \
../rt-thread/libcpu/mips/x1000/mips_context_gcc.S \
../rt-thread/libcpu/mips/x1000/mips_excpt_gcc.S \
../rt-thread/libcpu/mips/x1000/mips_fp_gcc.S \
../rt-thread/libcpu/mips/x1000/startup_gcc.S 

OBJS += \
./rt-thread/libcpu/mips/x1000/cache.o \
./rt-thread/libcpu/mips/x1000/cpu.o \
./rt-thread/libcpu/mips/x1000/interrupt.o \
./rt-thread/libcpu/mips/x1000/mips_backtrace.o \
./rt-thread/libcpu/mips/x1000/mips_cache_gcc.o \
./rt-thread/libcpu/mips/x1000/mips_context_gcc.o \
./rt-thread/libcpu/mips/x1000/mips_excpt.o \
./rt-thread/libcpu/mips/x1000/mips_excpt_gcc.o \
./rt-thread/libcpu/mips/x1000/mips_fp_gcc.o \
./rt-thread/libcpu/mips/x1000/stack.o \
./rt-thread/libcpu/mips/x1000/startup_gcc.o 

S_UPPER_DEPS += \
./rt-thread/libcpu/mips/x1000/mips_cache_gcc.d \
./rt-thread/libcpu/mips/x1000/mips_context_gcc.d \
./rt-thread/libcpu/mips/x1000/mips_excpt_gcc.d \
./rt-thread/libcpu/mips/x1000/mips_fp_gcc.d \
./rt-thread/libcpu/mips/x1000/startup_gcc.d 

C_DEPS += \
./rt-thread/libcpu/mips/x1000/cache.d \
./rt-thread/libcpu/mips/x1000/cpu.d \
./rt-thread/libcpu/mips/x1000/interrupt.d \
./rt-thread/libcpu/mips/x1000/mips_backtrace.d \
./rt-thread/libcpu/mips/x1000/mips_excpt.d \
./rt-thread/libcpu/mips/x1000/stack.d 


# Each subdirectory must supply rules for building sources it contributes
rt-thread/libcpu/mips/x1000/%.o: ../rt-thread/libcpu/mips/x1000/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -DRT_USING_NEWLIB -DSTM32F407xx -DUSE_HAL_DRIVER -I"D:\Documents\eclipse-workspace\theFive_app-rtt\applications" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\driver" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\Libraries\STM32F4xx_HAL_Driver\Inc" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\Libraries\CMSIS\Include" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\Libraries\CMSIS\Device\ST\STM32F4xx\Include" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\rt-thread\include" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\rt-thread\components\finsh" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

rt-thread/libcpu/mips/x1000/%.o: ../rt-thread/libcpu/mips/x1000/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross Assembler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -x assembler-with-cpp -Xassembler -mimplicit-it=thumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


