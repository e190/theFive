################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include rt-thread/src/subdir.mk
-include rt-thread/libcpu/arm/cortex-m4/subdir.mk
-include rt-thread/libcpu/arm/common/subdir.mk
-include rt-thread/components/utilities/ulog/syslog/subdir.mk
-include rt-thread/components/utilities/ulog/backend/subdir.mk
-include rt-thread/components/utilities/ulog/subdir.mk
-include rt-thread/components/utilities/cJSON-v1.0.2/subdir.mk
-include rt-thread/components/net/sal_socket/src/subdir.mk
-include rt-thread/components/net/sal_socket/socket/subdir.mk
-include rt-thread/components/net/sal_socket/impl/subdir.mk
-include rt-thread/components/net/at/src/subdir.mk
-include rt-thread/components/net/at/at_socket/subdir.mk
-include rt-thread/components/libc/compilers/newlib/subdir.mk
-include rt-thread/components/libc/compilers/common/subdir.mk
-include rt-thread/components/finsh/subdir.mk
-include rt-thread/components/drivers/usb/usbdevice/core/subdir.mk
-include rt-thread/components/drivers/usb/usbdevice/class/subdir.mk
-include rt-thread/components/drivers/src/subdir.mk
-include rt-thread/components/drivers/spi/sfud/src/subdir.mk
-include rt-thread/components/drivers/spi/subdir.mk
-include rt-thread/components/drivers/serial/subdir.mk
-include rt-thread/components/drivers/sdio/subdir.mk
-include rt-thread/components/drivers/rtc/subdir.mk
-include rt-thread/components/drivers/misc/subdir.mk
-include rt-thread/components/drivers/i2c/subdir.mk
-include rt-thread/components/drivers/cputime/subdir.mk
-include rt-thread/components/dfs/src/subdir.mk
-include rt-thread/components/dfs/filesystems/elmfat/option/subdir.mk
-include rt-thread/components/dfs/filesystems/elmfat/subdir.mk
-include rt-thread/components/dfs/filesystems/devfs/subdir.mk
-include components/flash/subdir.mk
-include components/fal/src/subdir.mk
-include components/cm_backtrace/subdir.mk
-include board/subdir.mk
-include applications/subdir.mk
-include Libraries/STM32F4xx_HAL/STM32F4xx_HAL_Driver/Src/subdir.mk
-include Libraries/STM32F4xx_HAL/CMSIS/Device/ST/STM32F4xx/Source/Templates/gcc/subdir.mk
-include Libraries/STM32F4xx_HAL/CMSIS/Device/ST/STM32F4xx/Source/Templates/subdir.mk
-include Libraries/HAL_Drivers/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
theFive_app-rtt.bin \

SECONDARY_SIZE += \
theFive_app-rtt.siz \


# All Target
all: theFive_app-rtt.elf secondary-outputs

# Tool invocations
theFive_app-rtt.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU ARM Cross C Linker'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -T "D:\Code\eclipse-workspace\theFive_app-rtt\STM32F407ZETx_FLASH.ld" -nostartfiles -Xlinker --gc-sections -Wl,-Map,"theFive_app-rtt.map" --specs=nano.specs -eentry -o "theFive_app-rtt.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

theFive_app-rtt.bin: theFive_app-rtt.elf
	@echo 'Invoking: GNU ARM Cross Create Flash Image'
	arm-none-eabi-objcopy -O binary "theFive_app-rtt.elf"  "theFive_app-rtt.bin"
	@echo 'Finished building: $@'
	@echo ' '

theFive_app-rtt.siz: theFive_app-rtt.elf
	@echo 'Invoking: GNU ARM Cross Print Size'
	arm-none-eabi-size --format=berkeley "theFive_app-rtt.elf"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(SECONDARY_FLASH)$(SECONDARY_SIZE)$(ASM_DEPS)$(S_UPPER_DEPS)$(C_DEPS) theFive_app-rtt.elf
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets
