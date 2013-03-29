################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Drivers/USB/Core/LPC/HCD/OHCI/subdir.mk
-include Drivers/USB/Core/LPC/HCD/EHCI/subdir.mk
-include Drivers/USB/Core/LPC/HCD/subdir.mk
-include Drivers/USB/Core/LPC/HAL/LPC18XX/subdir.mk
-include Drivers/USB/Core/LPC/HAL/LPC17XX/subdir.mk
-include Drivers/USB/Core/LPC/HAL/LPC11UXX/subdir.mk
-include Drivers/USB/Core/LPC/DCD/USBRom/subdir.mk
-include Drivers/USB/Core/LPC/DCD/LPC18XX/subdir.mk
-include Drivers/USB/Core/LPC/DCD/LPC17XX/subdir.mk
-include Drivers/USB/Core/LPC/DCD/LPC11UXX/subdir.mk
-include Drivers/USB/Core/LPC/subdir.mk
-include Drivers/USB/Core/subdir.mk
-include Drivers/USB/Class/Host/subdir.mk
-include Drivers/USB/Class/Device/subdir.mk
-include Drivers/USB/Class/Common/subdir.mk
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
all: pre-build main-build

# Main-build Target
main-build: libnxpUSBlib.a

# Tool invocations
libnxpUSBlib.a: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Archiver'
	arm-none-eabi-ar -r  "libnxpUSBlib.a" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(ARCHIVES) libnxpUSBlib.a
	-@echo ' '

pre-build:
	-if test -f "libnxpUSBlib.a" ; then cp -f "libnxpUSBlib.a" ../"libnxpUSBlib.a" ; fi
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "libnxpUSBlib.a" ; cp -f "libnxpUSBlib.a" ../"libnxpUSBlib.a" ; # arm-none-eabi-objdump -h -S "libnxpUSBlib.a" >"libnxpUSBlib.lss"
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: main-build pre-build post-build

-include ../makefile.targets
