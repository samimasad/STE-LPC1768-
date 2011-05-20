################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Drivers/source/lpc17xx_adc.c \
../src/Drivers/source/lpc17xx_can.c \
../src/Drivers/source/lpc17xx_clkpwr.c \
../src/Drivers/source/lpc17xx_dac.c \
../src/Drivers/source/lpc17xx_emac.c \
../src/Drivers/source/lpc17xx_gpdma.c \
../src/Drivers/source/lpc17xx_gpio.c \
../src/Drivers/source/lpc17xx_i2c.c \
../src/Drivers/source/lpc17xx_i2s.c \
../src/Drivers/source/lpc17xx_libcfg_default.c \
../src/Drivers/source/lpc17xx_mcpwm.c \
../src/Drivers/source/lpc17xx_nvic.c \
../src/Drivers/source/lpc17xx_pinsel.c \
../src/Drivers/source/lpc17xx_pwm.c \
../src/Drivers/source/lpc17xx_qei.c \
../src/Drivers/source/lpc17xx_rit.c \
../src/Drivers/source/lpc17xx_rtc.c \
../src/Drivers/source/lpc17xx_spi.c \
../src/Drivers/source/lpc17xx_ssp.c \
../src/Drivers/source/lpc17xx_timer.c \
../src/Drivers/source/lpc17xx_uart.c \
../src/Drivers/source/lpc17xx_wdt.c 

OBJS += \
./src/Drivers/source/lpc17xx_adc.o \
./src/Drivers/source/lpc17xx_can.o \
./src/Drivers/source/lpc17xx_clkpwr.o \
./src/Drivers/source/lpc17xx_dac.o \
./src/Drivers/source/lpc17xx_emac.o \
./src/Drivers/source/lpc17xx_gpdma.o \
./src/Drivers/source/lpc17xx_gpio.o \
./src/Drivers/source/lpc17xx_i2c.o \
./src/Drivers/source/lpc17xx_i2s.o \
./src/Drivers/source/lpc17xx_libcfg_default.o \
./src/Drivers/source/lpc17xx_mcpwm.o \
./src/Drivers/source/lpc17xx_nvic.o \
./src/Drivers/source/lpc17xx_pinsel.o \
./src/Drivers/source/lpc17xx_pwm.o \
./src/Drivers/source/lpc17xx_qei.o \
./src/Drivers/source/lpc17xx_rit.o \
./src/Drivers/source/lpc17xx_rtc.o \
./src/Drivers/source/lpc17xx_spi.o \
./src/Drivers/source/lpc17xx_ssp.o \
./src/Drivers/source/lpc17xx_timer.o \
./src/Drivers/source/lpc17xx_uart.o \
./src/Drivers/source/lpc17xx_wdt.o 

C_DEPS += \
./src/Drivers/source/lpc17xx_adc.d \
./src/Drivers/source/lpc17xx_can.d \
./src/Drivers/source/lpc17xx_clkpwr.d \
./src/Drivers/source/lpc17xx_dac.d \
./src/Drivers/source/lpc17xx_emac.d \
./src/Drivers/source/lpc17xx_gpdma.d \
./src/Drivers/source/lpc17xx_gpio.d \
./src/Drivers/source/lpc17xx_i2c.d \
./src/Drivers/source/lpc17xx_i2s.d \
./src/Drivers/source/lpc17xx_libcfg_default.d \
./src/Drivers/source/lpc17xx_mcpwm.d \
./src/Drivers/source/lpc17xx_nvic.d \
./src/Drivers/source/lpc17xx_pinsel.d \
./src/Drivers/source/lpc17xx_pwm.d \
./src/Drivers/source/lpc17xx_qei.d \
./src/Drivers/source/lpc17xx_rit.d \
./src/Drivers/source/lpc17xx_rtc.d \
./src/Drivers/source/lpc17xx_spi.d \
./src/Drivers/source/lpc17xx_ssp.d \
./src/Drivers/source/lpc17xx_timer.d \
./src/Drivers/source/lpc17xx_uart.d \
./src/Drivers/source/lpc17xx_wdt.d 


# Each subdirectory must supply rules for building sources it contributes
src/Drivers/source/%.o: ../src/Drivers/source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DNDEBUG -D__CODE_RED -D__REDLIB__ -O2 -g -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -mcpu=cortex-m3 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


