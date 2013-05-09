################################################################################
# Automatically-generated file. Do not edit!
################################################################################

#Progress monitor hint: 12
first : all
-include ../makefile.init

# This file contains definitions of environment variables used in the makefiles and .args files if exist.
-include makefile.local

RM := "$(GNU_Make_Install_DirEnv)/rm" -f

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include Source/subdir.mk
-include Source/Test/subdir.mk
-include Source/RouterSoftware/subdir.mk
-include Source/LocalBus/subdir.mk
-include Source/Etsec/subdir.mk
-include Source/Drv/subdir.mk
-include Source/Dma/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
linux.elf \

EXECUTABLES_QUOTED += \
"linux.elf" \

EXECUTABLES_OS_FORMAT += \
linux.elf \


# All Target
all: linux.elf

# Tool invocations
linux.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Executing target #13 $@'
	@echo 'Invoking: Power Linux Linker'
	"$(PAGccLinuxDirEnv)/powerpc-none-linux-gnuspe-gcc"  -o"linux.elf" $(OBJS_QUOTED) $(USER_OBJS_QUOTED) $(LIBS_QUOTED)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) "./*/*/*.o" "./*/*.o"  $(EXECUTABLES_QUOTED) "./*/*/*.d" "./*/*.d"  linux.elf
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
