################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/USB/Class/Common/HIDParser.c 

OBJS += \
./Drivers/USB/Class/Common/HIDParser.o 

C_DEPS += \
./Drivers/USB/Class/Common/HIDParser.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/USB/Class/Common/%.o: ../Drivers/USB/Class/Common/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__LPC17XX__ -DUSB_HOST_ONLY -D__REDLIB__ -D__CODE_RED -I"C:\Users\Sami\git\STE-LPC1768-\My_Example_With_Drivers\driver_include" -I"C:\Users\Sami\git\STE-LPC1768-\CMSISv2p00_LPC17xx\inc" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -std=gnu99 -mcpu=cortex-m3 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


