################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/I2S/I2S_test_4_wire/debug_frmwrk.c \
../src/I2S/I2S_test_4_wire/i2s_test_4_wire.c 

OBJS += \
./src/I2S/I2S_test_4_wire/debug_frmwrk.o \
./src/I2S/I2S_test_4_wire/i2s_test_4_wire.o 

C_DEPS += \
./src/I2S/I2S_test_4_wire/debug_frmwrk.d \
./src/I2S/I2S_test_4_wire/i2s_test_4_wire.d 


# Each subdirectory must supply rules for building sources it contributes
src/I2S/I2S_test_4_wire/%.o: ../src/I2S/I2S_test_4_wire/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -DPACK_STRUCT_END=__attribute\(\(packed\)\) -DALIGN_STRUCT_END=__attribute\(\(aligned\(4\)\)\) -D__CODE_RED -D__REDLIB__ -I"C:\Documents and Settings\nlv14991\Desktop\rtos\FreeRTOSV6.1.0\Demo\CORTEX_LPC1768_GCC_RedSuite\src\FreeRTOS\include" -I"C:\Documents and Settings\nlv14991\Desktop\rtos\FreeRTOSV6.1.0\Demo\CORTEX_LPC1768_GCC_RedSuite\src\LPCUSB" -I"C:\Documents and Settings\nlv14991\Desktop\rtos\FreeRTOSV6.1.0\Demo\CORTEX_LPC1768_GCC_RedSuite\src\LCD" -I"C:\Documents and Settings\nlv14991\Desktop\rtos\FreeRTOSV6.1.0\Demo\CORTEX_LPC1768_GCC_RedSuite\src\webserver" -I"C:\Documents and Settings\nlv14991\Desktop\rtos\FreeRTOSV6.1.0\Demo\CORTEX_LPC1768_GCC_RedSuite\src\Common Demo Tasks\include" -I"C:\Documents and Settings\nlv14991\Desktop\rtos\FreeRTOSV6.1.0\Demo\CORTEX_LPC1768_GCC_RedSuite\src" -I"C:\Documents and Settings\nlv14991\Desktop\rtos\FreeRTOSV6.1.0\Demo\CORTEX_LPC1768_GCC_RedSuite\src\FreeRTOS\portable\GCC\ARM_CM3" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -Wextra -mcpu=cortex-m3 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


