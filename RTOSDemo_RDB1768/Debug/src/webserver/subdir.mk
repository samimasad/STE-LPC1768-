################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/webserver/emac.c \
../src/webserver/http-strings.c \
../src/webserver/httpd-cgi.c \
../src/webserver/httpd-fs.c \
../src/webserver/httpd.c \
../src/webserver/psock.c \
../src/webserver/timer.c \
../src/webserver/uIP_Task.c \
../src/webserver/uip.c \
../src/webserver/uip_arp.c 

OBJS += \
./src/webserver/emac.o \
./src/webserver/http-strings.o \
./src/webserver/httpd-cgi.o \
./src/webserver/httpd-fs.o \
./src/webserver/httpd.o \
./src/webserver/psock.o \
./src/webserver/timer.o \
./src/webserver/uIP_Task.o \
./src/webserver/uip.o \
./src/webserver/uip_arp.o 

C_DEPS += \
./src/webserver/emac.d \
./src/webserver/http-strings.d \
./src/webserver/httpd-cgi.d \
./src/webserver/httpd-fs.d \
./src/webserver/httpd.d \
./src/webserver/psock.d \
./src/webserver/timer.d \
./src/webserver/uIP_Task.d \
./src/webserver/uip.d \
./src/webserver/uip_arp.d 


# Each subdirectory must supply rules for building sources it contributes
src/webserver/%.o: ../src/webserver/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__USE_CMSIS -DPACK_STRUCT_END=__attribute\(\(packed\)\) -DALIGN_STRUCT_END=__attribute\(\(aligned\(4\)\)\) -D__CODE_RED -D__REDLIB__ -I"C:\Documents and Settings\nlv14991\Desktop\rtos\FreeRTOSV6.1.0\Demo\CORTEX_LPC1768_GCC_RedSuite\src\FreeRTOS\include" -I"C:\Documents and Settings\nlv14991\Desktop\rtos\FreeRTOSV6.1.0\Demo\CORTEX_LPC1768_GCC_RedSuite\src\Drivers\include" -I"C:\Documents and Settings\nlv14991\Desktop\rtos\FreeRTOSV6.1.0\Demo\CORTEX_LPC1768_GCC_RedSuite\src\LPCUSB" -I"C:\Documents and Settings\nlv14991\Desktop\rtos\FreeRTOSV6.1.0\Demo\CORTEX_LPC1768_GCC_RedSuite\src\LCD" -I"C:\Documents and Settings\nlv14991\Desktop\rtos\FreeRTOSV6.1.0\Demo\CORTEX_LPC1768_GCC_RedSuite\src\webserver" -I"C:\Documents and Settings\nlv14991\Desktop\rtos\FreeRTOSV6.1.0\Demo\CORTEX_LPC1768_GCC_RedSuite\src\Common Demo Tasks\include" -I"C:\Documents and Settings\nlv14991\Desktop\rtos\FreeRTOSV6.1.0\Demo\CORTEX_LPC1768_GCC_RedSuite\src" -I"C:\Documents and Settings\nlv14991\Desktop\rtos\FreeRTOSV6.1.0\Demo\CORTEX_LPC1768_GCC_RedSuite\src\FreeRTOS\portable\GCC\ARM_CM3" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -Wextra -mcpu=cortex-m3 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


