################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../UART1_FullModem/uart_fullmodem_test.c 

OBJS += \
./UART1_FullModem/uart_fullmodem_test.o 

C_DEPS += \
./UART1_FullModem/uart_fullmodem_test.d 


# Each subdirectory must supply rules for building sources it contributes
UART1_FullModem/%.o: ../UART1_FullModem/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -D__USE_CMSIS -DDEBUG -D__CODE_RED -I"C:\Documents and Settings\nlv14991\My Documents\lpcxpresso_3.5\workspace\UART_TEST\DeviceSupport\NXP\LPC17xx\startup" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m3 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


