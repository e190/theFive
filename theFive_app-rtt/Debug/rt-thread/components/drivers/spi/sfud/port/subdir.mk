################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../rt-thread/components/drivers/spi/sfud/port/sfud_port.c 

OBJS += \
./rt-thread/components/drivers/spi/sfud/port/sfud_port.o 

C_DEPS += \
./rt-thread/components/drivers/spi/sfud/port/sfud_port.d 


# Each subdirectory must supply rules for building sources it contributes
rt-thread/components/drivers/spi/sfud/port/%.o: ../rt-thread/components/drivers/spi/sfud/port/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -DRT_USING_NEWLIB -DSTM32F407xx -DUSE_HAL_DRIVER -I"D:\Documents\eclipse-workspace\theFive_app-rtt\applications" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\rt-thread\components\drivers\include\drivers" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\rt-thread\packages\cJSON-v1.0.2" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\TMC5130" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\at\include" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\rt-thread\components\drivers\spi" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\rt-thread\components\drivers\spi\sfud\inc" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\rt-thread\components\drivers\include" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\driver" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\Libraries\STM32F4xx_HAL_Driver\Inc" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\Libraries\CMSIS\Include" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\Libraries\CMSIS\Device\ST\STM32F4xx\Include" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\rt-thread\include" -I"D:\Documents\eclipse-workspace\theFive_app-rtt\rt-thread\components\finsh" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


