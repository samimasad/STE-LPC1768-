################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../src/Core/CM3/DeviceSupport/NXP/LPC17xx/startup/arm/startup_LPC17xx.s 

OBJS += \
./src/Core/CM3/DeviceSupport/NXP/LPC17xx/startup/arm/startup_LPC17xx.o 


# Each subdirectory must supply rules for building sources it contributes
src/Core/CM3/DeviceSupport/NXP/LPC17xx/startup/arm/%.o: ../src/Core/CM3/DeviceSupport/NXP/LPC17xx/startup/arm/%.s
	@echo 'Building file: $<'
	@echo 'Invoking: MCU Assembler'
	arm-none-eabi-gcc -c -x assembler-with-cpp -DNDEBUG -D__CODE_RED -D__REDLIB__ -mcpu=cortex-m3 -mthumb -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


