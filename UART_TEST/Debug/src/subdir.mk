################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/core_cm3.c \
../src/cr_startup_lpc176x.c \
../src/lpc17xx_adc.c \
../src/lpc17xx_can.c \
../src/lpc17xx_clkpwr.c \
../src/lpc17xx_dac.c \
../src/lpc17xx_emac.c \
../src/lpc17xx_gpdma.c \
../src/lpc17xx_gpio.c \
../src/lpc17xx_i2c.c \
../src/lpc17xx_i2s.c \
../src/lpc17xx_libcfg_default.c \
../src/lpc17xx_mcpwm.c \
../src/lpc17xx_nvic.c \
../src/lpc17xx_pinsel.c \
../src/lpc17xx_pwm.c \
../src/lpc17xx_qei.c \
../src/lpc17xx_rit.c \
../src/lpc17xx_rtc.c \
../src/lpc17xx_spi.c \
../src/lpc17xx_ssp.c \
../src/lpc17xx_timer.c \
../src/lpc17xx_uart.c \
../src/lpc17xx_wdt.c \
../src/system_LPC17xx.c \
../src/uart_fullmodem_test.c 

OBJS += \
./src/core_cm3.o \
./src/cr_startup_lpc176x.o \
./src/lpc17xx_adc.o \
./src/lpc17xx_can.o \
./src/lpc17xx_clkpwr.o \
./src/lpc17xx_dac.o \
./src/lpc17xx_emac.o \
./src/lpc17xx_gpdma.o \
./src/lpc17xx_gpio.o \
./src/lpc17xx_i2c.o \
./src/lpc17xx_i2s.o \
./src/lpc17xx_libcfg_default.o \
./src/lpc17xx_mcpwm.o \
./src/lpc17xx_nvic.o \
./src/lpc17xx_pinsel.o \
./src/lpc17xx_pwm.o \
./src/lpc17xx_qei.o \
./src/lpc17xx_rit.o \
./src/lpc17xx_rtc.o \
./src/lpc17xx_spi.o \
./src/lpc17xx_ssp.o \
./src/lpc17xx_timer.o \
./src/lpc17xx_uart.o \
./src/lpc17xx_wdt.o \
./src/system_LPC17xx.o \
./src/uart_fullmodem_test.o 

C_DEPS += \
./src/core_cm3.d \
./src/cr_startup_lpc176x.d \
./src/lpc17xx_adc.d \
./src/lpc17xx_can.d \
./src/lpc17xx_clkpwr.d \
./src/lpc17xx_dac.d \
./src/lpc17xx_emac.d \
./src/lpc17xx_gpdma.d \
./src/lpc17xx_gpio.d \
./src/lpc17xx_i2c.d \
./src/lpc17xx_i2s.d \
./src/lpc17xx_libcfg_default.d \
./src/lpc17xx_mcpwm.d \
./src/lpc17xx_nvic.d \
./src/lpc17xx_pinsel.d \
./src/lpc17xx_pwm.d \
./src/lpc17xx_qei.d \
./src/lpc17xx_rit.d \
./src/lpc17xx_rtc.d \
./src/lpc17xx_spi.d \
./src/lpc17xx_ssp.d \
./src/lpc17xx_timer.d \
./src/lpc17xx_uart.d \
./src/lpc17xx_wdt.d \
./src/system_LPC17xx.d \
./src/uart_fullmodem_test.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -D__BUILD_WITH_EXAMPLE__=1 -D__RAM_MODE__=1 -D__USE_CMSIS -DDEBUG -D__CODE_RED -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m3 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


