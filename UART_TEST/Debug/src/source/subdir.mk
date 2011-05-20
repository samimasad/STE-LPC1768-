################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/source/lpc17xx_adc.c \
../src/source/lpc17xx_can.c \
../src/source/lpc17xx_clkpwr.c \
../src/source/lpc17xx_dac.c \
../src/source/lpc17xx_emac.c \
../src/source/lpc17xx_gpdma.c \
../src/source/lpc17xx_gpio.c \
../src/source/lpc17xx_i2c.c \
../src/source/lpc17xx_i2s.c \
../src/source/lpc17xx_libcfg_default.c \
../src/source/lpc17xx_mcpwm.c \
../src/source/lpc17xx_nvic.c \
../src/source/lpc17xx_pinsel.c \
../src/source/lpc17xx_pwm.c \
../src/source/lpc17xx_qei.c \
../src/source/lpc17xx_rit.c \
../src/source/lpc17xx_rtc.c \
../src/source/lpc17xx_spi.c \
../src/source/lpc17xx_ssp.c \
../src/source/lpc17xx_timer.c \
../src/source/lpc17xx_uart.c \
../src/source/lpc17xx_wdt.c 

OBJS += \
./src/source/lpc17xx_adc.o \
./src/source/lpc17xx_can.o \
./src/source/lpc17xx_clkpwr.o \
./src/source/lpc17xx_dac.o \
./src/source/lpc17xx_emac.o \
./src/source/lpc17xx_gpdma.o \
./src/source/lpc17xx_gpio.o \
./src/source/lpc17xx_i2c.o \
./src/source/lpc17xx_i2s.o \
./src/source/lpc17xx_libcfg_default.o \
./src/source/lpc17xx_mcpwm.o \
./src/source/lpc17xx_nvic.o \
./src/source/lpc17xx_pinsel.o \
./src/source/lpc17xx_pwm.o \
./src/source/lpc17xx_qei.o \
./src/source/lpc17xx_rit.o \
./src/source/lpc17xx_rtc.o \
./src/source/lpc17xx_spi.o \
./src/source/lpc17xx_ssp.o \
./src/source/lpc17xx_timer.o \
./src/source/lpc17xx_uart.o \
./src/source/lpc17xx_wdt.o 

C_DEPS += \
./src/source/lpc17xx_adc.d \
./src/source/lpc17xx_can.d \
./src/source/lpc17xx_clkpwr.d \
./src/source/lpc17xx_dac.d \
./src/source/lpc17xx_emac.d \
./src/source/lpc17xx_gpdma.d \
./src/source/lpc17xx_gpio.d \
./src/source/lpc17xx_i2c.d \
./src/source/lpc17xx_i2s.d \
./src/source/lpc17xx_libcfg_default.d \
./src/source/lpc17xx_mcpwm.d \
./src/source/lpc17xx_nvic.d \
./src/source/lpc17xx_pinsel.d \
./src/source/lpc17xx_pwm.d \
./src/source/lpc17xx_qei.d \
./src/source/lpc17xx_rit.d \
./src/source/lpc17xx_rtc.d \
./src/source/lpc17xx_spi.d \
./src/source/lpc17xx_ssp.d \
./src/source/lpc17xx_timer.d \
./src/source/lpc17xx_uart.d \
./src/source/lpc17xx_wdt.d 


# Each subdirectory must supply rules for building sources it contributes
src/source/%.o: ../src/source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -D__USE_CMSIS -DDEBUG -D__CODE_RED -I"C:\Documents and Settings\nlv14991\My Documents\lpcxpresso_3.5\workspace\UART_TEST\DeviceSupport\NXP\LPC17xx\startup" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m3 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


