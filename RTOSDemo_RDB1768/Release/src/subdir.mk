################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/ParTest.c \
../src/cr_startup_lpc17.c \
../src/main.c \
../src/printf-stdarg.c \
../src/syscalls.c 

OBJS += \
./src/ParTest.o \
./src/cr_startup_lpc17.o \
./src/main.o \
./src/printf-stdarg.o \
./src/syscalls.o 

C_DEPS += \
./src/ParTest.d \
./src/cr_startup_lpc17.d \
./src/main.d \
./src/printf-stdarg.d \
./src/syscalls.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DNDEBUG -D__CODE_RED -D__REDLIB__ -O2 -g -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -mcpu=cortex-m3 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


