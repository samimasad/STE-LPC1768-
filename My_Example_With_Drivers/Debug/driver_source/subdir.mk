################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../driver_source/lpc17xx_adc.c \
../driver_source/lpc17xx_can.c \
../driver_source/lpc17xx_clkpwr.c \
../driver_source/lpc17xx_dac.c \
../driver_source/lpc17xx_emac.c \
../driver_source/lpc17xx_gpdma.c \
../driver_source/lpc17xx_gpio.c \
../driver_source/lpc17xx_i2c.c \
../driver_source/lpc17xx_i2s.c \
../driver_source/lpc17xx_libcfg_default.c \
../driver_source/lpc17xx_mcpwm.c \
../driver_source/lpc17xx_nvic.c \
../driver_source/lpc17xx_pinsel.c \
../driver_source/lpc17xx_pwm.c \
../driver_source/lpc17xx_qei.c \
../driver_source/lpc17xx_rit.c \
../driver_source/lpc17xx_rtc.c \
../driver_source/lpc17xx_sd.c \
../driver_source/lpc17xx_spi.c \
../driver_source/lpc17xx_ssp.c \
../driver_source/lpc17xx_timer.c \
../driver_source/lpc17xx_uart.c \
../driver_source/lpc17xx_wdt.c 

OBJS += \
./driver_source/lpc17xx_adc.o \
./driver_source/lpc17xx_can.o \
./driver_source/lpc17xx_clkpwr.o \
./driver_source/lpc17xx_dac.o \
./driver_source/lpc17xx_emac.o \
./driver_source/lpc17xx_gpdma.o \
./driver_source/lpc17xx_gpio.o \
./driver_source/lpc17xx_i2c.o \
./driver_source/lpc17xx_i2s.o \
./driver_source/lpc17xx_libcfg_default.o \
./driver_source/lpc17xx_mcpwm.o \
./driver_source/lpc17xx_nvic.o \
./driver_source/lpc17xx_pinsel.o \
./driver_source/lpc17xx_pwm.o \
./driver_source/lpc17xx_qei.o \
./driver_source/lpc17xx_rit.o \
./driver_source/lpc17xx_rtc.o \
./driver_source/lpc17xx_sd.o \
./driver_source/lpc17xx_spi.o \
./driver_source/lpc17xx_ssp.o \
./driver_source/lpc17xx_timer.o \
./driver_source/lpc17xx_uart.o \
./driver_source/lpc17xx_wdt.o 

C_DEPS += \
./driver_source/lpc17xx_adc.d \
./driver_source/lpc17xx_can.d \
./driver_source/lpc17xx_clkpwr.d \
./driver_source/lpc17xx_dac.d \
./driver_source/lpc17xx_emac.d \
./driver_source/lpc17xx_gpdma.d \
./driver_source/lpc17xx_gpio.d \
./driver_source/lpc17xx_i2c.d \
./driver_source/lpc17xx_i2s.d \
./driver_source/lpc17xx_libcfg_default.d \
./driver_source/lpc17xx_mcpwm.d \
./driver_source/lpc17xx_nvic.d \
./driver_source/lpc17xx_pinsel.d \
./driver_source/lpc17xx_pwm.d \
./driver_source/lpc17xx_qei.d \
./driver_source/lpc17xx_rit.d \
./driver_source/lpc17xx_rtc.d \
./driver_source/lpc17xx_sd.d \
./driver_source/lpc17xx_spi.d \
./driver_source/lpc17xx_ssp.d \
./driver_source/lpc17xx_timer.d \
./driver_source/lpc17xx_uart.d \
./driver_source/lpc17xx_wdt.d 


# Each subdirectory must supply rules for building sources it contributes
driver_source/%.o: ../driver_source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -DFATFS_FLAG -DPACK_STRUCT_END=__attribute\(\(packed\)\) -DALIGN_STRUCT_END=__attribute\(\(aligned\(4\)\)\) -D__USE_CMSIS=CMSISv2p00_LPC17xx -D__CODE_RED -DLED_TICK -D__NEWLIB__ -DAES_ENABLE -UHTTPD_FS_STATISTICS=1 -I"C:\Users\Sami\git\STE-LPC1768-\FreeRTOS_Library\demo_code" -I"C:\Users\Sami\git\STE-LPC1768-\aes_lib\inc" -I"C:\Users\Sami\git\STE-LPC1768-\My_Example_With_Drivers\webserver" -I"C:\Users\Sami\git\STE-LPC1768-\My_Example_With_Drivers\uip_ftp" -I"C:\Users\Sami\git\STE-LPC1768-\My_Example_With_Drivers\FatFs" -I"C:\Users\Sami\git\STE-LPC1768-\My_Example_With_Drivers\UsbHost\Include" -I"C:\Users\Sami\git\STE-LPC1768-\My_Example_With_Drivers\UsbHost\Host" -I"C:\Users\Sami\git\STE-LPC1768-\My_Example_With_Drivers\UsbHost\MassStorage" -I"C:\Users\Sami\git\STE-LPC1768-\My_Example_With_Drivers\UsbHost\Uart" -I"C:\Users\Sami\git\STE-LPC1768-\My_Example_With_Drivers\driver_include" -I"C:\Users\Sami\git\STE-LPC1768-\CMSISv2p00_LPC17xx\inc" -I"C:\Users\Sami\git\STE-LPC1768-\FreeRTOS_Library\include" -I"C:\Users\Sami\git\STE-LPC1768-\FreeRTOS_Library\portable" -O3 -g3 -fsigned-char -c -fmessage-length=0 -fno-builtin -ffunction-sections -mcpu=cortex-m3 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


