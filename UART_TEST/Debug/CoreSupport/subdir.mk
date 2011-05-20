################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CoreSupport/core_cm3.c 

OBJS += \
./CoreSupport/core_cm3.o 

C_DEPS += \
./CoreSupport/core_cm3.d 


# Each subdirectory must supply rules for building sources it contributes
CoreSupport/%.o: ../CoreSupport/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -D__USE_CMSIS -DDEBUG -D__CODE_RED -I"C:\Documents and Settings\nlv14991\My Documents\lpcxpresso_3.5\workspace\UART_TEST\DeviceSupport\NXP\LPC17xx\startup" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m3 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


