################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/LCD/lcd.c \
../src/LCD/lcd_driver.c \
../src/LCD/system_fixed_be_8_15.c 

OBJS += \
./src/LCD/lcd.o \
./src/LCD/lcd_driver.o \
./src/LCD/system_fixed_be_8_15.o 

C_DEPS += \
./src/LCD/lcd.d \
./src/LCD/lcd_driver.d \
./src/LCD/system_fixed_be_8_15.d 


# Each subdirectory must supply rules for building sources it contributes
src/LCD/%.o: ../src/LCD/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DNDEBUG -D__CODE_RED -D__REDLIB__ -O2 -g -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -mcpu=cortex-m3 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


