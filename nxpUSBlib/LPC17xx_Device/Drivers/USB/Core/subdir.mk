################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/USB/Core/ConfigDescriptor.c \
../Drivers/USB/Core/DeviceStandardReq.c \
../Drivers/USB/Core/Events.c \
../Drivers/USB/Core/HostStandardReq.c \
../Drivers/USB/Core/USBMemory.c \
../Drivers/USB/Core/USBTask.c 

OBJS += \
./Drivers/USB/Core/ConfigDescriptor.o \
./Drivers/USB/Core/DeviceStandardReq.o \
./Drivers/USB/Core/Events.o \
./Drivers/USB/Core/HostStandardReq.o \
./Drivers/USB/Core/USBMemory.o \
./Drivers/USB/Core/USBTask.o 

C_DEPS += \
./Drivers/USB/Core/ConfigDescriptor.d \
./Drivers/USB/Core/DeviceStandardReq.d \
./Drivers/USB/Core/Events.d \
./Drivers/USB/Core/HostStandardReq.d \
./Drivers/USB/Core/USBMemory.d \
./Drivers/USB/Core/USBTask.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/USB/Core/%.o: ../Drivers/USB/Core/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__LPC17XX__ -DUSB_DEVICE_ONLY -D__CODE_RED -D__REDLIB__ -I"C:\Users\Sami\Documents\Workspace\Lib_CMSISv2p00_LPC17xx\inc" -I"C:\Users\Sami\Documents\Workspace\Lib_MCU\inc" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -std=gnu99 -mcpu=cortex-m3 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


