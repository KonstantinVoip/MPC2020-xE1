################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Source/Etsec/mpcapp_menu_etsec.c" 

C_SRCS += \
../Source/Etsec/mpcapp_menu_etsec.c 

OBJS += \
./Source/Etsec/mpcapp_menu_etsec.o 

C_DEPS += \
./Source/Etsec/mpcapp_menu_etsec.d 

OBJS_QUOTED += \
"./Source/Etsec/mpcapp_menu_etsec.o" 

OBJS_OS_FORMAT += \
./Source/Etsec/mpcapp_menu_etsec.o 

C_DEPS_QUOTED += \
"./Source/Etsec/mpcapp_menu_etsec.d" 


# Each subdirectory must supply rules for building sources it contributes
Source/Etsec/%.o: ../Source/Etsec/%.c
	@echo 'Building file: $<'
	@echo 'Executing target #9 $<'
	@echo 'Invoking: Power Linux Compiler'
	"$(PAGccLinuxDirEnv)/powerpc-none-linux-gnuspe-gcc" -I"$(ProjDirPath)/include" -O0 -g -Wall -c -fmessage-length=0 -mhard-float -te500v2 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


