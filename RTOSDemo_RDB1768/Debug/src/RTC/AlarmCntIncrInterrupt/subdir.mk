################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/RTC/AlarmCntIncrInterrupt/debug_frmwrk.c \
../src/RTC/AlarmCntIncrInterrupt/libnosys_gnu.c \
../src/RTC/AlarmCntIncrInterrupt/rtc_alarm_cntincr_int.c 

OBJS += \
./src/RTC/AlarmCntIncrInterrupt/debug_frmwrk.o \
./src/RTC/AlarmCntIncrInterrupt/libnosys_gnu.o \
./src/RTC/AlarmCntIncrInterrupt/rtc_alarm_cntincr_int.o 

C_DEPS += \
./src/RTC/AlarmCntIncrInterrupt/debug_frmwrk.d \
./src/RTC/AlarmCntIncrInterrupt/libnosys_gnu.d \
./src/RTC/AlarmCntIncrInterrupt/rtc_alarm_cntincr_int.d 


# Each subdirectory must supply rules for building sources it contributes
src/RTC/AlarmCntIncrInterrupt/%.o: ../src/RTC/AlarmCntIncrInterrupt/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -DPACK_STRUCT_END=__attribute\(\(packed\)\) -DALIGN_STRUCT_END=__attribute\(\(aligned\(4\)\)\) -D__CODE_RED -D__REDLIB__ -I"C:\Documents and Settings\nlv14991\Desktop\rtos\FreeRTOSV6.1.0\Demo\CORTEX_LPC1768_GCC_RedSuite\src\FreeRTOS\include" -I"C:\Documents and Settings\nlv14991\Desktop\rtos\FreeRTOSV6.1.0\Demo\CORTEX_LPC1768_GCC_RedSuite\src\LPCUSB" -I"C:\Documents and Settings\nlv14991\Desktop\rtos\FreeRTOSV6.1.0\Demo\CORTEX_LPC1768_GCC_RedSuite\src\LCD" -I"C:\Documents and Settings\nlv14991\Desktop\rtos\FreeRTOSV6.1.0\Demo\CORTEX_LPC1768_GCC_RedSuite\src\webserver" -I"C:\Documents and Settings\nlv14991\Desktop\rtos\FreeRTOSV6.1.0\Demo\CORTEX_LPC1768_GCC_RedSuite\src\Common Demo Tasks\include" -I"C:\Documents and Settings\nlv14991\Desktop\rtos\FreeRTOSV6.1.0\Demo\CORTEX_LPC1768_GCC_RedSuite\src" -I"C:\Documents and Settings\nlv14991\Desktop\rtos\FreeRTOSV6.1.0\Demo\CORTEX_LPC1768_GCC_RedSuite\src\FreeRTOS\portable\GCC\ARM_CM3" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -Wextra -mcpu=cortex-m3 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


