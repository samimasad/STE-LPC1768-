################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include webserver/subdir.mk
-include uip_ftp/subdir.mk
-include driver_source/subdir.mk
-include Zigbee/subdir.mk
-include UsbHost/Uart/subdir.mk
-include UsbHost/MassStorage/subdir.mk
-include UsbHost/Main/subdir.mk
-include UsbHost/Host/subdir.mk
-include LPCUSB/subdir.mk
-include FatFs/option/subdir.mk
-include FatFs/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: My_Example_With_Drivers.axf

# Tool invocations
My_Example_With_Drivers.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"C:\Users\Sami\git\STE-LPC1768-\CMSISv2p00_LPC17xx\Debug" -L"C:\Users\Sami\git\STE-LPC1768-\aes_lib\Debug" -L"C:\Users\Sami\git\STE-LPC1768-\FreeRTOS_Library\Debug" -Xlinker --gc-sections -Xlinker -Map=My_Example_With_Drivers.map -mcpu=cortex-m3 -mthumb -T "link_template.ld" -L "C:\Users\Sami\git\STE-LPC1768-\My_Example_With_Drivers\linkscripts" -o "My_Example_With_Drivers.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) My_Example_With_Drivers.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size My_Example_With_Drivers.axf; # arm-none-eabi-objcopy -O ihex My_Example_With_Drivers.axf My_Example_With_Drivers.hex ;
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
