################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/GPDMA/Mem_2_Mem_Test/debug_frmwrk.c \
../src/GPDMA/Mem_2_Mem_Test/gpdma_m2m_test.c 

OBJS += \
./src/GPDMA/Mem_2_Mem_Test/debug_frmwrk.o \
./src/GPDMA/Mem_2_Mem_Test/gpdma_m2m_test.o 

C_DEPS += \
./src/GPDMA/Mem_2_Mem_Test/debug_frmwrk.d \
./src/GPDMA/Mem_2_Mem_Test/gpdma_m2m_test.d 


# Each subdirectory must supply rules for building sources it contributes
src/GPDMA/Mem_2_Mem_Test/%.o: ../src/GPDMA/Mem_2_Mem_Test/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -DPACK_STRUCT_END=__attribute\(\(packed\)\) -DALIGN_STRUCT_END=__attribute\(\(aligned\(4\)\)\) -D__CODE_RED -D__REDLIB__ -I"C:\Documents and Settings\nlv14991\Desktop\rtos\FreeRTOSV6.1.0\Demo\CORTEX_LPC1768_GCC_RedSuite\src\FreeRTOS\include" -I"C:\Documents and Settings\nlv14991\Desktop\rtos\FreeRTOSV6.1.0\Demo\CORTEX_LPC1768_GCC_RedSuite\src\LPCUSB" -I"C:\Documents and Settings\nlv14991\Desktop\rtos\FreeRTOSV6.1.0\Demo\CORTEX_LPC1768_GCC_RedSuite\src\LCD" -I"C:\Documents and Settings\nlv14991\Desktop\rtos\FreeRTOSV6.1.0\Demo\CORTEX_LPC1768_GCC_RedSuite\src\webserver" -I"C:\Documents and Settings\nlv14991\Desktop\rtos\FreeRTOSV6.1.0\Demo\CORTEX_LPC1768_GCC_RedSuite\src\Common Demo Tasks\include" -I"C:\Documents and Settings\nlv14991\Desktop\rtos\FreeRTOSV6.1.0\Demo\CORTEX_LPC1768_GCC_RedSuite\src" -I"C:\Documents and Settings\nlv14991\Desktop\rtos\FreeRTOSV6.1.0\Demo\CORTEX_LPC1768_GCC_RedSuite\src\FreeRTOS\portable\GCC\ARM_CM3" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -Wextra -mcpu=cortex-m3 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


