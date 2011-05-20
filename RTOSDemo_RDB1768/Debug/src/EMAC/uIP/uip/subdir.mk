################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/EMAC/uIP/uip/psock.c \
../src/EMAC/uIP/uip/timer.c \
../src/EMAC/uIP/uip/uip-fw.c \
../src/EMAC/uIP/uip/uip-neighbor.c \
../src/EMAC/uIP/uip/uip-split.c \
../src/EMAC/uIP/uip/uip.c \
../src/EMAC/uIP/uip/uip_arp.c \
../src/EMAC/uIP/uip/uiplib.c 

OBJS += \
./src/EMAC/uIP/uip/psock.o \
./src/EMAC/uIP/uip/timer.o \
./src/EMAC/uIP/uip/uip-fw.o \
./src/EMAC/uIP/uip/uip-neighbor.o \
./src/EMAC/uIP/uip/uip-split.o \
./src/EMAC/uIP/uip/uip.o \
./src/EMAC/uIP/uip/uip_arp.o \
./src/EMAC/uIP/uip/uiplib.o 

C_DEPS += \
./src/EMAC/uIP/uip/psock.d \
./src/EMAC/uIP/uip/timer.d \
./src/EMAC/uIP/uip/uip-fw.d \
./src/EMAC/uIP/uip/uip-neighbor.d \
./src/EMAC/uIP/uip/uip-split.d \
./src/EMAC/uIP/uip/uip.d \
./src/EMAC/uIP/uip/uip_arp.d \
./src/EMAC/uIP/uip/uiplib.d 


# Each subdirectory must supply rules for building sources it contributes
src/EMAC/uIP/uip/%.o: ../src/EMAC/uIP/uip/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -DPACK_STRUCT_END=__attribute\(\(packed\)\) -DALIGN_STRUCT_END=__attribute\(\(aligned\(4\)\)\) -D__CODE_RED -D__REDLIB__ -I"C:\Documents and Settings\nlv14991\Desktop\rtos\FreeRTOSV6.1.0\Demo\CORTEX_LPC1768_GCC_RedSuite\src\FreeRTOS\include" -I"C:\Documents and Settings\nlv14991\Desktop\rtos\FreeRTOSV6.1.0\Demo\CORTEX_LPC1768_GCC_RedSuite\src\LPCUSB" -I"C:\Documents and Settings\nlv14991\Desktop\rtos\FreeRTOSV6.1.0\Demo\CORTEX_LPC1768_GCC_RedSuite\src\LCD" -I"C:\Documents and Settings\nlv14991\Desktop\rtos\FreeRTOSV6.1.0\Demo\CORTEX_LPC1768_GCC_RedSuite\src\webserver" -I"C:\Documents and Settings\nlv14991\Desktop\rtos\FreeRTOSV6.1.0\Demo\CORTEX_LPC1768_GCC_RedSuite\src\Common Demo Tasks\include" -I"C:\Documents and Settings\nlv14991\Desktop\rtos\FreeRTOSV6.1.0\Demo\CORTEX_LPC1768_GCC_RedSuite\src" -I"C:\Documents and Settings\nlv14991\Desktop\rtos\FreeRTOSV6.1.0\Demo\CORTEX_LPC1768_GCC_RedSuite\src\FreeRTOS\portable\GCC\ARM_CM3" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -Wextra -mcpu=cortex-m3 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


