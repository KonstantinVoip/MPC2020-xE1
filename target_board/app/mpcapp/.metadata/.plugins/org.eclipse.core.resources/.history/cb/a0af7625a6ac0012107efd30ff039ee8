################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Source/mpcapp_linux.c" \
"../Source/mpcapp_main.c" \
"../Source/mpcapp_menu.c" \
"../Source/mpcapp_user.c" 

C_SRCS += \
../Source/mpcapp_linux.c \
../Source/mpcapp_main.c \
../Source/mpcapp_menu.c \
../Source/mpcapp_user.c 

OBJS += \
./Source/mpcapp_linux.o \
./Source/mpcapp_main.o \
./Source/mpcapp_menu.o \
./Source/mpcapp_user.o 

C_DEPS += \
./Source/mpcapp_linux.d \
./Source/mpcapp_main.d \
./Source/mpcapp_menu.d \
./Source/mpcapp_user.d 

OBJS_QUOTED += \
"./Source/mpcapp_linux.o" \
"./Source/mpcapp_main.o" \
"./Source/mpcapp_menu.o" \
"./Source/mpcapp_user.o" 

OBJS_OS_FORMAT += \
./Source/mpcapp_linux.o \
./Source/mpcapp_main.o \
./Source/mpcapp_menu.o \
./Source/mpcapp_user.o 

C_DEPS_QUOTED += \
"./Source/mpcapp_linux.d" \
"./Source/mpcapp_main.d" \
"./Source/mpcapp_menu.d" \
"./Source/mpcapp_user.d" 


# Each subdirectory must supply rules for building sources it contributes
Source/%.o: ../Source/%.c
	@echo 'Building file: $<'
	@echo 'Executing target #1 $<'
	@echo 'Invoking: Power Linux Compiler'
	"$(PAGccLinuxDirEnv)/powerpc-none-linux-gnuspe-gcc" -I"$(ProjDirPath)/include" -O0 -g -Wall -c -fmessage-length=0 -mhard-float -te500v2 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


