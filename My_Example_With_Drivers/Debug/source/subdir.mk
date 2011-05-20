################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/lpc17xx_adc.c \
../source/lpc17xx_can.c \
../source/lpc17xx_clkpwr.c \
../source/lpc17xx_dac.c \
../source/lpc17xx_emac.c \
../source/lpc17xx_gpdma.c \
../source/lpc17xx_gpio.c \
../source/lpc17xx_i2c.c \
../source/lpc17xx_i2s.c \
../source/lpc17xx_libcfg_default.c \
../source/lpc17xx_mcpwm.c \
../source/lpc17xx_nvic.c \
../source/lpc17xx_pinsel.c \
../source/lpc17xx_pwm.c \
../source/lpc17xx_qei.c \
../source/lpc17xx_rit.c \
../source/lpc17xx_rtc.c \
../source/lpc17xx_spi.c \
../source/lpc17xx_ssp.c \
../source/lpc17xx_timer.c \
../source/lpc17xx_uart.c \
../source/lpc17xx_wdt.c 

OBJS += \
./source/lpc17xx_adc.o \
./source/lpc17xx_can.o \
./source/lpc17xx_clkpwr.o \
./source/lpc17xx_dac.o \
./source/lpc17xx_emac.o \
./source/lpc17xx_gpdma.o \
./source/lpc17xx_gpio.o \
./source/lpc17xx_i2c.o \
./source/lpc17xx_i2s.o \
./source/lpc17xx_libcfg_default.o \
./source/lpc17xx_mcpwm.o \
./source/lpc17xx_nvic.o \
./source/lpc17xx_pinsel.o \
./source/lpc17xx_pwm.o \
./source/lpc17xx_qei.o \
./source/lpc17xx_rit.o \
./source/lpc17xx_rtc.o \
./source/lpc17xx_spi.o \
./source/lpc17xx_ssp.o \
./source/lpc17xx_timer.o \
./source/lpc17xx_uart.o \
./source/lpc17xx_wdt.o 

C_DEPS += \
./source/lpc17xx_adc.d \
./source/lpc17xx_can.d \
./source/lpc17xx_clkpwr.d \
./source/lpc17xx_dac.d \
./source/lpc17xx_emac.d \
./source/lpc17xx_gpdma.d \
./source/lpc17xx_gpio.d \
./source/lpc17xx_i2c.d \
./source/lpc17xx_i2s.d \
./source/lpc17xx_libcfg_default.d \
./source/lpc17xx_mcpwm.d \
./source/lpc17xx_nvic.d \
./source/lpc17xx_pinsel.d \
./source/lpc17xx_pwm.d \
./source/lpc17xx_qei.d \
./source/lpc17xx_rit.d \
./source/lpc17xx_rtc.d \
./source/lpc17xx_spi.d \
./source/lpc17xx_ssp.d \
./source/lpc17xx_timer.d \
./source/lpc17xx_uart.d \
./source/lpc17xx_wdt.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -DPACK_STRUCT_END=__attribute\(\(packed\)\) -DALIGN_STRUCT_END=__attribute\(\(aligned\(4\)\)\) -D__USE_CMSIS=CMSISv1p30_LPC17xx -D__CODE_RED -D__REDLIB__ -DLED_TICK -UHTTPD_FS_STATISTICS=1 -I"C:\Documents and Settings\nlv14991\My Documents\lpcxpresso_3.5\workspace\FreeRTOS_Library\demo_code" -I"C:\Documents and Settings\nlv14991\My Documents\lpcxpresso_3.5\workspace\My_Example_With_Drivers\UsbHost\Include" -I"C:\Documents and Settings\nlv14991\My Documents\lpcxpresso_3.5\workspace\My_Example_With_Drivers\UsbHost\Fat" -I"C:\Documents and Settings\nlv14991\My Documents\lpcxpresso_3.5\workspace\My_Example_With_Drivers\UsbHost\Host" -I"C:\Documents and Settings\nlv14991\My Documents\lpcxpresso_3.5\workspace\My_Example_With_Drivers\UsbHost\MassStorage" -I"C:\Documents and Settings\nlv14991\My Documents\lpcxpresso_3.5\workspace\My_Example_With_Drivers\UsbHost\Uart" -I"C:\Documents and Settings\nlv14991\My Documents\lpcxpresso_3.5\workspace\My_Example_With_Drivers\include" -I"C:\Documents and Settings\nlv14991\My Documents\lpcxpresso_3.5\workspace\CMSISv1p30_LPC17xx\inc" -I"C:\Documents and Settings\nlv14991\My Documents\lpcxpresso_3.5\workspace\FreeRTOS_Library\include" -I"C:\Documents and Settings\nlv14991\My Documents\lpcxpresso_3.5\workspace\FreeRTOS_Library\portable" -O0 -g3 -fsigned-char -c -fmessage-length=0 -fno-builtin -ffunction-sections -mcpu=cortex-m3 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


