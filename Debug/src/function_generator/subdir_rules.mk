################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
src/function_generator/sine_wave.obj: ../src/function_generator/sine_wave.cpp $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.2.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me -Ooff --opt_for_speed=5 --fp_mode=relaxed --include_path="C:/Users/Anthony/workspace_v8/Inverter" --include_path="C:/ti/simplelink_msp432e4_sdk_2_10_00_17/source/third_party/CMSIS/Include" --include_path="C:/ti/simplelink_msp432e4_sdk_2_10_00_17/source/ti/posix/ccs" --include_path="C:/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.2.LTS/include" --advice:power=none --define=__FPU_PRESENT=1 --define=ARM_MATH_CM4 --define=__MSP432E401Y__ --define=DeviceFamily_MSP432E -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="src/function_generator/sine_wave.d_raw" --obj_directory="src/function_generator" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


