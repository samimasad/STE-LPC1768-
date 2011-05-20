################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Core/CM3/CoreSupport/core_cm3.c 

OBJS += \
./src/Core/CM3/CoreSupport/core_cm3.o 

C_DEPS += \
./src/Core/CM3/CoreSupport/core_cm3.d 


# Each subdirectory must supply rules for building sources it contributes
src/Core/CM3/CoreSupport/%.o: ../src/Core/CM3/CoreSupport/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DNDEBUG -D__CODE_RED -D__REDLIB__ -O2 -g -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -mcpu=cortex-m3 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


