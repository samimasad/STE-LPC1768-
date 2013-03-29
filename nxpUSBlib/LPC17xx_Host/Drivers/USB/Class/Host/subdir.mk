################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/USB/Class/Host/AudioClassHost.c \
../Drivers/USB/Class/Host/CDCClassHost.c \
../Drivers/USB/Class/Host/HIDClassHost.c \
../Drivers/USB/Class/Host/MIDIClassHost.c \
../Drivers/USB/Class/Host/MassStorageClassHost.c \
../Drivers/USB/Class/Host/PrinterClassHost.c \
../Drivers/USB/Class/Host/RNDISClassHost.c \
../Drivers/USB/Class/Host/StillImageClassHost.c 

OBJS += \
./Drivers/USB/Class/Host/AudioClassHost.o \
./Drivers/USB/Class/Host/CDCClassHost.o \
./Drivers/USB/Class/Host/HIDClassHost.o \
./Drivers/USB/Class/Host/MIDIClassHost.o \
./Drivers/USB/Class/Host/MassStorageClassHost.o \
./Drivers/USB/Class/Host/PrinterClassHost.o \
./Drivers/USB/Class/Host/RNDISClassHost.o \
./Drivers/USB/Class/Host/StillImageClassHost.o 

C_DEPS += \
./Drivers/USB/Class/Host/AudioClassHost.d \
./Drivers/USB/Class/Host/CDCClassHost.d \
./Drivers/USB/Class/Host/HIDClassHost.d \
./Drivers/USB/Class/Host/MIDIClassHost.d \
./Drivers/USB/Class/Host/MassStorageClassHost.d \
./Drivers/USB/Class/Host/PrinterClassHost.d \
./Drivers/USB/Class/Host/RNDISClassHost.d \
./Drivers/USB/Class/Host/StillImageClassHost.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/USB/Class/Host/%.o: ../Drivers/USB/Class/Host/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__LPC17XX__ -DUSB_HOST_ONLY -D__REDLIB__ -D__CODE_RED -I"C:\Users\Sami\Documents\Workspace\Lib_CMSISv2p00_LPC17xx\inc" -I"C:\Users\Sami\Documents\Workspace\Lib_MCU\inc" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -std=gnu99 -mcpu=cortex-m3 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


