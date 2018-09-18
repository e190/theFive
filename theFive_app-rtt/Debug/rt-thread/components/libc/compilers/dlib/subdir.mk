################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../rt-thread/components/libc/compilers/dlib/environ.c \
../rt-thread/components/libc/compilers/dlib/libc.c \
../rt-thread/components/libc/compilers/dlib/rmtx.c \
../rt-thread/components/libc/compilers/dlib/stdio.c \
../rt-thread/components/libc/compilers/dlib/syscall_close.c \
../rt-thread/components/libc/compilers/dlib/syscall_lseek.c \
../rt-thread/components/libc/compilers/dlib/syscall_mem.c \
../rt-thread/components/libc/compilers/dlib/syscall_open.c \
../rt-thread/components/libc/compilers/dlib/syscall_read.c \
../rt-thread/components/libc/compilers/dlib/syscall_remove.c \
../rt-thread/components/libc/compilers/dlib/syscall_write.c \
../rt-thread/components/libc/compilers/dlib/time.c 

OBJS += \
./rt-thread/components/libc/compilers/dlib/environ.o \
./rt-thread/components/libc/compilers/dlib/libc.o \
./rt-thread/components/libc/compilers/dlib/rmtx.o \
./rt-thread/components/libc/compilers/dlib/stdio.o \
./rt-thread/components/libc/compilers/dlib/syscall_close.o \
./rt-thread/components/libc/compilers/dlib/syscall_lseek.o \
./rt-thread/components/libc/compilers/dlib/syscall_mem.o \
./rt-thread/components/libc/compilers/dlib/syscall_open.o \
./rt-thread/components/libc/compilers/dlib/syscall_read.o \
./rt-thread/components/libc/compilers/dlib/syscall_remove.o \
./rt-thread/components/libc/compilers/dlib/syscall_write.o \
./rt-thread/components/libc/compilers/dlib/time.o 

C_DEPS += \
./rt-thread/components/libc/compilers/dlib/environ.d \
./rt-thread/components/libc/compilers/dlib/libc.d \
./rt-thread/components/libc/compilers/dlib/rmtx.d \
./rt-thread/components/libc/compilers/dlib/stdio.d \
./rt-thread/components/libc/compilers/dlib/syscall_close.d \
./rt-thread/components/libc/compilers/dlib/syscall_lseek.d \
./rt-thread/components/libc/compilers/dlib/syscall_mem.d \
./rt-thread/components/libc/compilers/dlib/syscall_open.d \
./rt-thread/components/libc/compilers/dlib/syscall_read.d \
./rt-thread/components/libc/compilers/dlib/syscall_remove.d \
./rt-thread/components/libc/compilers/dlib/syscall_write.d \
./rt-thread/components/libc/compilers/dlib/time.d 


# Each subdirectory must supply rules for building sources it contributes
rt-thread/components/libc/compilers/dlib/%.o: ../rt-thread/components/libc/compilers/dlib/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -DRT_USING_NEWLIB -DSTM32F407xx -DUSE_HAL_DRIVER -I"D:\Documents\eclipse-workspace\theFive_app-rtt\applications" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\driver" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\Libraries\STM32F4xx_HAL_Driver\Inc" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\Libraries\CMSIS\Include" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\Libraries\CMSIS\Device\ST\STM32F4xx\Include" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\rt-thread\include" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\rt-thread\components\finsh" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


