################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Source/RouterSoftware/mpcapp_menu_softrouter.c" 

C_SRCS += \
../Source/RouterSoftware/mpcapp_menu_softrouter.c 

OBJS += \
./Source/RouterSoftware/mpcapp_menu_softrouter.o 

C_DEPS += \
./Source/RouterSoftware/mpcapp_menu_softrouter.d 

OBJS_QUOTED += \
"./Source/RouterSoftware/mpcapp_menu_softrouter.o" 

OBJS_OS_FORMAT += \
./Source/RouterSoftware/mpcapp_menu_softrouter.o 

C_DEPS_QUOTED += \
"./Source/RouterSoftware/mpcapp_menu_softrouter.d" 


# Each subdirectory must supply rules for building sources it contributes
Source/RouterSoftware/%.o: ../Source/RouterSoftware/%.c
	@echo 'Building file: $<'
	@echo 'Executing target #7 $<'
	@echo 'Invoking: Power Linux Compiler'
	"$(PAGccLinuxDirEnv)/powerpc-none-linux-gnuspe-gcc" -I"$(ProjDirPath)/include" -O0 -g -Wall -c -fmessage-length=0 -mhard-float -te500v2 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


