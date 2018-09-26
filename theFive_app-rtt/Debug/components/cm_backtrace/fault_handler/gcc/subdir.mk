################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_UPPER_SRCS += \
../components/cm_backtrace/fault_handler/gcc/cmb_fault.S 

OBJS += \
./components/cm_backtrace/fault_handler/gcc/cmb_fault.o 

S_UPPER_DEPS += \
./components/cm_backtrace/fault_handler/gcc/cmb_fault.d 


# Each subdirectory must supply rules for building sources it contributes
components/cm_backtrace/fault_handler/gcc/%.o: ../components/cm_backtrace/fault_handler/gcc/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross Assembler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -x assembler-with-cpp -DDEBUG -Xassembler -mimplicit-it=thumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


