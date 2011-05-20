################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include src/webserver/subdir.mk
-include src/subdir.mk
-include src/LPCUSB/subdir.mk
-include src/LCD/subdir.mk
-include src/FreeRTOS/subdir.mk
-include src/FreeRTOS/portable/MemMang/subdir.mk
-include src/FreeRTOS/portable/GCC/ARM_CM3/subdir.mk
-include src/Drivers/source/subdir.mk
-include src/Core/CM3/DeviceSupport/NXP/LPC17xx/subdir.mk
-include src/Core/CM3/DeviceSupport/NXP/LPC17xx/startup/arm/subdir.mk
-include src/Core/CM3/CoreSupport/subdir.mk
-include src/Common\ Demo\ Tasks/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: RTOSDemo_RDB1768.axf

# Tool invocations
RTOSDemo_RDB1768.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -Xlinker --gc-sections -Xlinker -Map=RTOSDemo_RDB1768.map -mcpu=cortex-m3 -mthumb -T "rtosdemo_rdb1768_Release.ld" -o"RTOSDemo_RDB1768.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) RTOSDemo_RDB1768.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size RTOSDemo_RDB1768.axf; # arm-none-eabi-objdump -h -S RTOSDemo_RDB1768.axf >RTOSDemo_RDB1768.lss
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
