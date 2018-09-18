################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../rt-thread/components/drivers/usb/usbhost/core/core.c \
../rt-thread/components/drivers/usb/usbhost/core/driver.c \
../rt-thread/components/drivers/usb/usbhost/core/hub.c \
../rt-thread/components/drivers/usb/usbhost/core/usbhost.c 

OBJS += \
./rt-thread/components/drivers/usb/usbhost/core/core.o \
./rt-thread/components/drivers/usb/usbhost/core/driver.o \
./rt-thread/components/drivers/usb/usbhost/core/hub.o \
./rt-thread/components/drivers/usb/usbhost/core/usbhost.o 

C_DEPS += \
./rt-thread/components/drivers/usb/usbhost/core/core.d \
./rt-thread/components/drivers/usb/usbhost/core/driver.d \
./rt-thread/components/drivers/usb/usbhost/core/hub.d \
./rt-thread/components/drivers/usb/usbhost/core/usbhost.d 


# Each subdirectory must supply rules for building sources it contributes
rt-thread/components/drivers/usb/usbhost/core/%.o: ../rt-thread/components/drivers/usb/usbhost/core/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -DRT_USING_NEWLIB -DSTM32F407xx -DUSE_HAL_DRIVER -I"D:\Documents\eclipse-workspace\theFive_app-rtt\applications" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\rt-thread\components\drivers\include" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\driver" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\Libraries\STM32F4xx_HAL_Driver\Inc" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\Libraries\CMSIS\Include" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\Libraries\CMSIS\Device\ST\STM32F4xx\Include" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\rt-thread\include" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\rt-thread\components\finsh" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


