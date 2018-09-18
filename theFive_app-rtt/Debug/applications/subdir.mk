################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../applications/AnalogToDigital.c \
../applications/Heat_PID.c \
../applications/RunLED.c \
../applications/SaveData.c \
../applications/ServerData.c \
../applications/Uart_Screen.c \
../applications/WorkTask.c \
../applications/main.c 

OBJS += \
./applications/AnalogToDigital.o \
./applications/Heat_PID.o \
./applications/RunLED.o \
./applications/SaveData.o \
./applications/ServerData.o \
./applications/Uart_Screen.o \
./applications/WorkTask.o \
./applications/main.o 

C_DEPS += \
./applications/AnalogToDigital.d \
./applications/Heat_PID.d \
./applications/RunLED.d \
./applications/SaveData.d \
./applications/ServerData.d \
./applications/Uart_Screen.d \
./applications/WorkTask.d \
./applications/main.d 


# Each subdirectory must supply rules for building sources it contributes
applications/%.o: ../applications/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -DRT_USING_NEWLIB -DDEBUG -DSTM32F407xx -DUSE_HAL_DRIVER -I"D:\Code\eclipse-workspace\theFive_app-rtt\applications" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\drivers\include\drivers" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\packages\cJSON-v1.0.2" -I"D:\Code\eclipse-workspace\theFive_app-rtt\TMC5130" -I"D:\Code\eclipse-workspace\theFive_app-rtt\at\include" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\drivers\spi" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\drivers\spi\sfud\inc" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\drivers\include" -I"D:\Code\eclipse-workspace\theFive_app-rtt\driver" -I"D:\Code\eclipse-workspace\theFive_app-rtt\Libraries\STM32F4xx_HAL_Driver\Inc" -I"D:\Code\eclipse-workspace\theFive_app-rtt\Libraries\CMSIS\Include" -I"D:\Code\eclipse-workspace\theFive_app-rtt\Libraries\CMSIS\Device\ST\STM32F4xx\Include" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\include" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\finsh" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


