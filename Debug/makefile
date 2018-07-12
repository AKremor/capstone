################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.2.LTS

GEN_OPTS += C:/Users/Anthony/workspace_v8/tirtos_builds_MSP_EXP432E401Y_release_ccs/Debug/configPkg/compiler.opt
GEN_OPTS__FLAG := --cmd_file="C:/Users/Anthony/workspace_v8/tirtos_builds_MSP_EXP432E401Y_release_ccs/Debug/configPkg/compiler.opt"
GEN_CMDS += C:/Users/Anthony/workspace_v8/tirtos_builds_MSP_EXP432E401Y_release_ccs/Debug/configPkg/linker.cmd
GEN_CMDS__FLAG := -l"C:/Users/Anthony/workspace_v8/tirtos_builds_MSP_EXP432E401Y_release_ccs/Debug/configPkg/linker.cmd"

ORDERED_OBJS += \
"./MSP_EXP432E401Y.obj" \
"./empty.obj" \
"./main_tirtos.obj" \
"./src/mpc/mpc.obj" \
"../MSP_EXP432E401Y_TIRTOS.cmd" \
-l"ti/display/lib/display.aem4f" \
-l"ti/grlib/lib/ccs/m4f/grlib.a" \
-l"third_party/spiffs/lib/ccs/m4f/spiffs.a" \
-l"ti/drivers/lib/drivers_msp432e4.aem4f" \
-l"third_party/fatfs/lib/ccs/m4f/fatfs.a" \
-l"ti/dpl/lib/dpl_msp432e4.aem4f" \
$(GEN_CMDS__FLAG) \
-l"ti/devices/msp432e4/driverlib/lib/ccs/m4f/msp432e4_driverlib.a" \
-llibc.a \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include src/mpc/subdir_vars.mk
-include subdir_rules.mk
-include src/mpc/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
Inverter.out \

EXE_OUTPUTS__QUOTED += \
"Inverter.out" \

BIN_OUTPUTS += \
Inverter.hex \

BIN_OUTPUTS__QUOTED += \
"Inverter.hex" \


# All Target
all: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@$(MAKE) --no-print-directory -Onone "Inverter.out"

dependents:
	-cd C:/Users/Anthony/workspace_v8/tirtos_builds_MSP_EXP432E401Y_release_ccs/Debug && $(MAKE) all

# Tool invocations
Inverter.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS) C:/Users/Anthony/workspace_v8/tirtos_builds_MSP_EXP432E401Y_release_ccs/Debug/tirtos_builds_MSP_EXP432E401Y_release_ccs.lib
	@echo 'Building target: "$@"'
	@echo 'Invoking: ARM Linker'
	"C:/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.2.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me -O3 --opt_for_speed=5 --fp_mode=relaxed --advice:power=none --define=__FPU_PRESENT=1 --define=ARM_MATH_CM4 --define=__MSP432E401Y__ --define=DeviceFamily_MSP432E -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on -z -m"Inverter.map" -i"C:/ti/simplelink_msp432e4_sdk_2_10_00_17/source" -i"C:/ti/simplelink_msp432e4_sdk_2_10_00_17/kernel/tirtos/packages" -i"C:/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.2.LTS/lib" --diag_wrap=off --display_error_number --warn_sections --xml_link_info="Inverter_linkInfo.xml" --rom_model -o "Inverter.out" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '

Inverter.hex: $(EXE_OUTPUTS)
	@echo 'Building files: $(strip $(EXE_OUTPUTS__QUOTED))'
	@echo 'Invoking: ARM Hex Utility'
	"C:/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.2.LTS/bin/armhex"  -o "Inverter.hex" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building: $(strip $(EXE_OUTPUTS__QUOTED))'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(BIN_OUTPUTS__QUOTED)$(EXE_OUTPUTS__QUOTED)
	-$(RM) "MSP_EXP432E401Y.obj" "empty.obj" "main_tirtos.obj" "src\mpc\mpc.obj" 
	-$(RM) "MSP_EXP432E401Y.d" "main_tirtos.d" 
	-$(RM) "empty.d" "src\mpc\mpc.d" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:
C:/Users/Anthony/workspace_v8/tirtos_builds_MSP_EXP432E401Y_release_ccs/Debug/tirtos_builds_MSP_EXP432E401Y_release_ccs.lib:

-include ../makefile.targets

