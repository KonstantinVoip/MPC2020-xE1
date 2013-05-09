################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Source/Drv/mpcapp_menu_p2020drvReg.c" \
"../Source/Drv/mpcapp_p2020drvReg.c" 

C_SRCS += \
../Source/Drv/mpcapp_menu_p2020drvReg.c \
../Source/Drv/mpcapp_p2020drvReg.c 

OBJS += \
./Source/Drv/mpcapp_menu_p2020drvReg.o \
./Source/Drv/mpcapp_p2020drvReg.o 

C_DEPS += \
./Source/Drv/mpcapp_menu_p2020drvReg.d \
./Source/Drv/mpcapp_p2020drvReg.d 

OBJS_QUOTED += \
"./Source/Drv/mpcapp_menu_p2020drvReg.o" \
"./Source/Drv/mpcapp_p2020drvReg.o" 

OBJS_OS_FORMAT += \
./Source/Drv/mpcapp_menu_p2020drvReg.o \
./Source/Drv/mpcapp_p2020drvReg.o 

C_DEPS_QUOTED += \
"./Source/Drv/mpcapp_menu_p2020drvReg.d" \
"./Source/Drv/mpcapp_p2020drvReg.d" 


# Each subdirectory must supply rules for building sources it contributes
Source/Drv/%.o: ../Source/Drv/%.c
	@echo 'Building file: $<'
	@echo 'Executing target #10 $<'
	@echo 'Invoking: Power Linux Compiler'
	"$(PAGccLinuxDirEnv)/powerpc-none-linux-gnuspe-gcc" -I"$(ProjDirPath)/include" -O0 -g -Wall -c -fmessage-length=0 -mhard-float -te500v2 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


