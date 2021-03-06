################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../rt-thread/src/clock.c \
../rt-thread/src/components.c \
../rt-thread/src/cpu.c \
../rt-thread/src/device.c \
../rt-thread/src/idle.c \
../rt-thread/src/ipc.c \
../rt-thread/src/irq.c \
../rt-thread/src/kservice.c \
../rt-thread/src/mem.c \
../rt-thread/src/memheap.c \
../rt-thread/src/mempool.c \
../rt-thread/src/object.c \
../rt-thread/src/scheduler.c \
../rt-thread/src/signal.c \
../rt-thread/src/slab.c \
../rt-thread/src/thread.c \
../rt-thread/src/timer.c 

OBJS += \
./rt-thread/src/clock.o \
./rt-thread/src/components.o \
./rt-thread/src/cpu.o \
./rt-thread/src/device.o \
./rt-thread/src/idle.o \
./rt-thread/src/ipc.o \
./rt-thread/src/irq.o \
./rt-thread/src/kservice.o \
./rt-thread/src/mem.o \
./rt-thread/src/memheap.o \
./rt-thread/src/mempool.o \
./rt-thread/src/object.o \
./rt-thread/src/scheduler.o \
./rt-thread/src/signal.o \
./rt-thread/src/slab.o \
./rt-thread/src/thread.o \
./rt-thread/src/timer.o 

C_DEPS += \
./rt-thread/src/clock.d \
./rt-thread/src/components.d \
./rt-thread/src/cpu.d \
./rt-thread/src/device.d \
./rt-thread/src/idle.d \
./rt-thread/src/ipc.d \
./rt-thread/src/irq.d \
./rt-thread/src/kservice.d \
./rt-thread/src/mem.d \
./rt-thread/src/memheap.d \
./rt-thread/src/mempool.d \
./rt-thread/src/object.d \
./rt-thread/src/scheduler.d \
./rt-thread/src/signal.d \
./rt-thread/src/slab.d \
./rt-thread/src/thread.d \
./rt-thread/src/timer.d 


# Each subdirectory must supply rules for building sources it contributes
rt-thread/src/%.o: ../rt-thread/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -DSTM32F407xx -DHAVE_SYS_SELECT_H -DUSE_HAL_DRIVER -I"D:\Code\eclipse-workspace\theFive_app-rtt\Libraries\HAL_Drivers" -I"D:\Code\eclipse-workspace\theFive_app-rtt\components\fal\inc" -I"D:\Code\eclipse-workspace\theFive_app-rtt\Libraries\HAL_Drivers\config" -I"D:\Code\eclipse-workspace\theFive_app-rtt\Libraries\STM32F4xx_HAL\CMSIS\Device\ST\STM32F4xx\Include" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\utilities\ulog\backend" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\utilities\ulog\syslog" -I"D:\Code\eclipse-workspace\theFive_app-rtt\components\cm_backtrace" -I"D:\Code\eclipse-workspace\theFive_app-rtt\components\flash" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\drivers\spi" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\drivers\spi\sfud\inc" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\drivers\usb\usbdevice\class" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\net\at\include" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\net\sal_socket\include\dfs_net" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\net\sal_socket\impl" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\net\at\at_socket" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\utilities\logtrace" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\net\sal_socket\include\socket\sys_socket" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\net\sal_socket\include" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\net\sal_socket\include\socket" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\utilities\ulog" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\utilities\cJSON-v1.0.2" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\utilities\webclient-v2.0.1\inc" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\dfs\include" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\finsh" -I"D:\Code\eclipse-workspace\theFive_app-rtt\Libraries\STM32F4xx_HAL\CMSIS\Include" -I"D:\Code\eclipse-workspace\theFive_app-rtt\Libraries\STM32F4xx_HAL\STM32F4xx_HAL_Driver\Inc" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\include" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\drivers\include" -I"D:\Code\eclipse-workspace\theFive_app-rtt\board" -I"D:\Code\eclipse-workspace\theFive_app-rtt\applications" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


