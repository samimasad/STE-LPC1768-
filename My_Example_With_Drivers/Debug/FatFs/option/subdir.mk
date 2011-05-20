################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../FatFs/option/cc932.c \
../FatFs/option/syncobj.c \
../FatFs/option/syscall.c 

OBJS += \
./FatFs/option/cc932.o \
./FatFs/option/syncobj.o \
./FatFs/option/syscall.o 

C_DEPS += \
./FatFs/option/cc932.d \
./FatFs/option/syncobj.d \
./FatFs/option/syscall.d 


# Each subdirectory must supply rules for building sources it contributes
FatFs/option/%.o: ../FatFs/option/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -DFATFS_FLAG -DPACK_STRUCT_END=__attribute\(\(packed\)\) -DALIGN_STRUCT_END=__attribute\(\(aligned\(4\)\)\) -D__USE_CMSIS=CMSISv1p30_LPC17xx -D__CODE_RED -D__REDLIB__ -DLED_TICK -UHTTPD_FS_STATISTICS=1 -I"C:\Documents and Settings\nlv14991\My Documents\lpcxpresso_3.5\workspace\FreeRTOS_Library\demo_code" -I"C:\Documents and Settings\nlv14991\My Documents\lpcxpresso_3.5\workspace\My_Example_With_Drivers\webserver" -I"C:\Documents and Settings\nlv14991\My Documents\lpcxpresso_3.5\workspace\My_Example_With_Drivers\uip_ftp" -I"C:\Documents and Settings\nlv14991\My Documents\lpcxpresso_3.5\workspace\My_Example_With_Drivers\FatFs" -I"C:\Documents and Settings\nlv14991\My Documents\lpcxpresso_3.5\workspace\My_Example_With_Drivers\UsbHost\Include" -I"C:\Documents and Settings\nlv14991\My Documents\lpcxpresso_3.5\workspace\My_Example_With_Drivers\UsbHost\Host" -I"C:\Documents and Settings\nlv14991\My Documents\lpcxpresso_3.5\workspace\My_Example_With_Drivers\UsbHost\MassStorage" -I"C:\Documents and Settings\nlv14991\My Documents\lpcxpresso_3.5\workspace\My_Example_With_Drivers\UsbHost\Uart" -I"C:\Documents and Settings\nlv14991\My Documents\lpcxpresso_3.5\workspace\My_Example_With_Drivers\driver_include" -I"C:\Documents and Settings\nlv14991\My Documents\lpcxpresso_3.5\workspace\CMSISv1p30_LPC17xx\inc" -I"C:\Documents and Settings\nlv14991\My Documents\lpcxpresso_3.5\workspace\FreeRTOS_Library\include" -I"C:\Documents and Settings\nlv14991\My Documents\lpcxpresso_3.5\workspace\FreeRTOS_Library\portable" -O0 -g3 -fsigned-char -c -fmessage-length=0 -fno-builtin -ffunction-sections -mcpu=cortex-m3 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


