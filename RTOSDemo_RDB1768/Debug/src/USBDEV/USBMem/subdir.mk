################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/USBDEV/USBMem/DiskImg.c \
../src/USBDEV/USBMem/memory.c \
../src/USBDEV/USBMem/mscuser.c \
../src/USBDEV/USBMem/usbcore.c \
../src/USBDEV/USBMem/usbdesc.c \
../src/USBDEV/USBMem/usbhw.c \
../src/USBDEV/USBMem/usbuser.c 

OBJS += \
./src/USBDEV/USBMem/DiskImg.o \
./src/USBDEV/USBMem/memory.o \
./src/USBDEV/USBMem/mscuser.o \
./src/USBDEV/USBMem/usbcore.o \
./src/USBDEV/USBMem/usbdesc.o \
./src/USBDEV/USBMem/usbhw.o \
./src/USBDEV/USBMem/usbuser.o 

C_DEPS += \
./src/USBDEV/USBMem/DiskImg.d \
./src/USBDEV/USBMem/memory.d \
./src/USBDEV/USBMem/mscuser.d \
./src/USBDEV/USBMem/usbcore.d \
./src/USBDEV/USBMem/usbdesc.d \
./src/USBDEV/USBMem/usbhw.d \
./src/USBDEV/USBMem/usbuser.d 


# Each subdirectory must supply rules for building sources it contributes
src/USBDEV/USBMem/%.o: ../src/USBDEV/USBMem/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -DPACK_STRUCT_END=__attribute\(\(packed\)\) -DALIGN_STRUCT_END=__attribute\(\(aligned\(4\)\)\) -D__CODE_RED -D__REDLIB__ -I"C:\Documents and Settings\nlv14991\Desktop\rtos\FreeRTOSV6.1.0\Demo\CORTEX_LPC1768_GCC_RedSuite\src\FreeRTOS\include" -I"C:\Documents and Settings\nlv14991\Desktop\rtos\FreeRTOSV6.1.0\Demo\CORTEX_LPC1768_GCC_RedSuite\src\LPCUSB" -I"C:\Documents and Settings\nlv14991\Desktop\rtos\FreeRTOSV6.1.0\Demo\CORTEX_LPC1768_GCC_RedSuite\src\LCD" -I"C:\Documents and Settings\nlv14991\Desktop\rtos\FreeRTOSV6.1.0\Demo\CORTEX_LPC1768_GCC_RedSuite\src\webserver" -I"C:\Documents and Settings\nlv14991\Desktop\rtos\FreeRTOSV6.1.0\Demo\CORTEX_LPC1768_GCC_RedSuite\src\Common Demo Tasks\include" -I"C:\Documents and Settings\nlv14991\Desktop\rtos\FreeRTOSV6.1.0\Demo\CORTEX_LPC1768_GCC_RedSuite\src" -I"C:\Documents and Settings\nlv14991\Desktop\rtos\FreeRTOSV6.1.0\Demo\CORTEX_LPC1768_GCC_RedSuite\src\FreeRTOS\portable\GCC\ARM_CM3" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -Wextra -mcpu=cortex-m3 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


