################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/EMAC/uIP/apps/webserver/http-strings.c \
../src/EMAC/uIP/apps/webserver/httpd-cgi.c \
../src/EMAC/uIP/apps/webserver/httpd-fs.c \
../src/EMAC/uIP/apps/webserver/httpd-fsdata.c \
../src/EMAC/uIP/apps/webserver/httpd.c 

OBJS += \
./src/EMAC/uIP/apps/webserver/http-strings.o \
./src/EMAC/uIP/apps/webserver/httpd-cgi.o \
./src/EMAC/uIP/apps/webserver/httpd-fs.o \
./src/EMAC/uIP/apps/webserver/httpd-fsdata.o \
./src/EMAC/uIP/apps/webserver/httpd.o 

C_DEPS += \
./src/EMAC/uIP/apps/webserver/http-strings.d \
./src/EMAC/uIP/apps/webserver/httpd-cgi.d \
./src/EMAC/uIP/apps/webserver/httpd-fs.d \
./src/EMAC/uIP/apps/webserver/httpd-fsdata.d \
./src/EMAC/uIP/apps/webserver/httpd.d 


# Each subdirectory must supply rules for building sources it contributes
src/EMAC/uIP/apps/webserver/%.o: ../src/EMAC/uIP/apps/webserver/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -DPACK_STRUCT_END=__attribute\(\(packed\)\) -DALIGN_STRUCT_END=__attribute\(\(aligned\(4\)\)\) -D__CODE_RED -D__REDLIB__ -I"C:\Documents and Settings\nlv14991\Desktop\rtos\FreeRTOSV6.1.0\Demo\CORTEX_LPC1768_GCC_RedSuite\src\FreeRTOS\include" -I"C:\Documents and Settings\nlv14991\Desktop\rtos\FreeRTOSV6.1.0\Demo\CORTEX_LPC1768_GCC_RedSuite\src\LPCUSB" -I"C:\Documents and Settings\nlv14991\Desktop\rtos\FreeRTOSV6.1.0\Demo\CORTEX_LPC1768_GCC_RedSuite\src\LCD" -I"C:\Documents and Settings\nlv14991\Desktop\rtos\FreeRTOSV6.1.0\Demo\CORTEX_LPC1768_GCC_RedSuite\src\webserver" -I"C:\Documents and Settings\nlv14991\Desktop\rtos\FreeRTOSV6.1.0\Demo\CORTEX_LPC1768_GCC_RedSuite\src\Common Demo Tasks\include" -I"C:\Documents and Settings\nlv14991\Desktop\rtos\FreeRTOSV6.1.0\Demo\CORTEX_LPC1768_GCC_RedSuite\src" -I"C:\Documents and Settings\nlv14991\Desktop\rtos\FreeRTOSV6.1.0\Demo\CORTEX_LPC1768_GCC_RedSuite\src\FreeRTOS\portable\GCC\ARM_CM3" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -Wextra -mcpu=cortex-m3 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


