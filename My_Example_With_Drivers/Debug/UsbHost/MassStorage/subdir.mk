################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../UsbHost/MassStorage/usbhost_ms.c 

OBJS += \
./UsbHost/MassStorage/usbhost_ms.o 

C_DEPS += \
./UsbHost/MassStorage/usbhost_ms.d 


# Each subdirectory must supply rules for building sources it contributes
UsbHost/MassStorage/%.o: ../UsbHost/MassStorage/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -DFATFS_FLAG -DPACK_STRUCT_END=__attribute\(\(packed\)\) -DALIGN_STRUCT_END=__attribute\(\(aligned\(4\)\)\) -D__USE_CMSIS=CMSISv1p30_LPC17xx -D__CODE_RED -DLED_TICK -D__NEWLIB__ -UHTTPD_FS_STATISTICS=1 -I"/home/nlv14991/workspaceLPC/STE-LPC1768-/FreeRTOS_Library/demo_code" -I"/home/nlv14991/workspaceLPC/STE-LPC1768-/My_Example_With_Drivers/webserver" -I"/home/nlv14991/workspaceLPC/STE-LPC1768-/My_Example_With_Drivers/uip_ftp" -I"/home/nlv14991/workspaceLPC/STE-LPC1768-/My_Example_With_Drivers/FatFs" -I"/home/nlv14991/workspaceLPC/STE-LPC1768-/My_Example_With_Drivers/UsbHost/Include" -I"/home/nlv14991/workspaceLPC/STE-LPC1768-/My_Example_With_Drivers/UsbHost/Host" -I"/home/nlv14991/workspaceLPC/STE-LPC1768-/My_Example_With_Drivers/UsbHost/MassStorage" -I"/home/nlv14991/workspaceLPC/STE-LPC1768-/My_Example_With_Drivers/UsbHost/Uart" -I"/home/nlv14991/workspaceLPC/STE-LPC1768-/My_Example_With_Drivers/driver_include" -I"/home/nlv14991/workspaceLPC/STE-LPC1768-/CMSISv1p30_LPC17xx/inc" -I"/home/nlv14991/workspaceLPC/STE-LPC1768-/FreeRTOS_Library/include" -I"/home/nlv14991/workspaceLPC/STE-LPC1768-/FreeRTOS_Library/portable" -O3 -g3 -fsigned-char -c -fmessage-length=0 -fno-builtin -ffunction-sections -mcpu=cortex-m3 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


