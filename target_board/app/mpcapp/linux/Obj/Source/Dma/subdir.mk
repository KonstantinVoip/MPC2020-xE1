################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Source/Dma/mpcapp_menu_dma.c" 

C_SRCS += \
../Source/Dma/mpcapp_menu_dma.c 

OBJS += \
./Source/Dma/mpcapp_menu_dma.o 

C_DEPS += \
./Source/Dma/mpcapp_menu_dma.d 

OBJS_QUOTED += \
"./Source/Dma/mpcapp_menu_dma.o" 

OBJS_OS_FORMAT += \
./Source/Dma/mpcapp_menu_dma.o 

C_DEPS_QUOTED += \
"./Source/Dma/mpcapp_menu_dma.d" 


# Each subdirectory must supply rules for building sources it contributes
Source/Dma/%.o: ../Source/Dma/%.c
	@echo 'Building file: $<'
	@echo 'Executing target #12 $<'
	@echo 'Invoking: Power Linux Compiler'
	"$(PAGccLinuxDirEnv)/powerpc-none-linux-gnuspe-gcc" -I"$(ProjDirPath)/include" -O0 -g -Wall -c -fmessage-length=0 -mhard-float -te500v2 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


