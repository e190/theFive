################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c \
../Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c \
../Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c \
../Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma2d.c \
../Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c \
../Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c \
../Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pcd.c \
../Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pcd_ex.c \
../Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c \
../Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c \
../Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c \
../Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c \
../Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_spi.c \
../Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c \
../Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.c \
../Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c \
../Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_usart.c \
../Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_usb.c 

OBJS += \
./Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.o \
./Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.o \
./Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.o \
./Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma2d.o \
./Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.o \
./Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.o \
./Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pcd.o \
./Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pcd_ex.o \
./Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.o \
./Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.o \
./Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.o \
./Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.o \
./Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_spi.o \
./Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.o \
./Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.o \
./Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.o \
./Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_usart.o \
./Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_usb.o 

C_DEPS += \
./Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.d \
./Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.d \
./Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.d \
./Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma2d.d \
./Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.d \
./Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.d \
./Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pcd.d \
./Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pcd_ex.d \
./Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.d \
./Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.d \
./Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.d \
./Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.d \
./Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_spi.d \
./Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.d \
./Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.d \
./Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.d \
./Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_usart.d \
./Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_usb.d 


# Each subdirectory must supply rules for building sources it contributes
Libraries/STM32F4xx_HAL_Driver/Src/%.o: ../Libraries/STM32F4xx_HAL_Driver/Src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -DRT_USING_NEWLIB -DDEBUG -DSTM32F407xx -DUSE_HAL_DRIVER -I"D:\Code\eclipse-workspace\theFive_app-rtt\applications" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\drivers\include\drivers" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\packages\cJSON-v1.0.2" -I"D:\Code\eclipse-workspace\theFive_app-rtt\TMC5130" -I"D:\Code\eclipse-workspace\theFive_app-rtt\at\include" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\drivers\spi" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\drivers\spi\sfud\inc" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\drivers\include" -I"D:\Code\eclipse-workspace\theFive_app-rtt\driver" -I"D:\Code\eclipse-workspace\theFive_app-rtt\Libraries\STM32F4xx_HAL_Driver\Inc" -I"D:\Code\eclipse-workspace\theFive_app-rtt\Libraries\CMSIS\Include" -I"D:\Code\eclipse-workspace\theFive_app-rtt\Libraries\CMSIS\Device\ST\STM32F4xx\Include" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\include" -I"D:\Code\eclipse-workspace\theFive_app-rtt\rt-thread\components\finsh" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


