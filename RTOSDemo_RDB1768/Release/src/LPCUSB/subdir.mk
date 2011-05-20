################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/LPCUSB/USB_CDC.c \
../src/LPCUSB/usbcontrol.c \
../src/LPCUSB/usbhw_lpc.c \
../src/LPCUSB/usbinit.c \
../src/LPCUSB/usbstdreq.c 

OBJS += \
./src/LPCUSB/USB_CDC.o \
./src/LPCUSB/usbcontrol.o \
./src/LPCUSB/usbhw_lpc.o \
./src/LPCUSB/usbinit.o \
./src/LPCUSB/usbstdreq.o 

C_DEPS += \
./src/LPCUSB/USB_CDC.d \
./src/LPCUSB/usbcontrol.d \
./src/LPCUSB/usbhw_lpc.d \
./src/LPCUSB/usbinit.d \
./src/LPCUSB/usbstdreq.d 


# Each subdirectory must supply rules for building sources it contributes
src/LPCUSB/%.o: ../src/LPCUSB/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DNDEBUG -D__CODE_RED -D__REDLIB__ -O2 -g -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -mcpu=cortex-m3 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


