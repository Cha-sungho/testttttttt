################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
phoenixon/utils/%.obj: ../phoenixon/utils/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs1031/ccs/tools/compiler/ti-cgt-c2000_21.6.0.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 -Ooff --fp_mode=strict --include_path="C:/ti/ccs1031/ccs/tools/compiler/ti-cgt-c2000_21.6.0.LTS/include" --include_path="C:/Users/PHOENIXON/Documents/11121E-00_11122E-00_NorthVolt_5V100A_Formation_Channel/Channel Board/power_cpu1" --include_path="C:/Users/PHOENIXON/Documents/11121E-00_11122E-00_NorthVolt_5V100A_Formation_Channel/Channel Board/power_cpu1/easyDSP" --include_path="C:/Users/PHOENIXON/Documents/11121E-00_11122E-00_NorthVolt_5V100A_Formation_Channel/Channel Board/power_cpu1/dsp/c2000_sdk/v400/f2837xd" --include_path="C:/Users/PHOENIXON/Documents/11121E-00_11122E-00_NorthVolt_5V100A_Formation_Channel/Channel Board/power_cpu1/dsp/c2000_sdk/v400/f2837xd/common" --include_path="C:/Users/PHOENIXON/Documents/11121E-00_11122E-00_NorthVolt_5V100A_Formation_Channel/Channel Board/power_cpu1/dsp/c2000_sdk/v400/f2837xd/device" --include_path="C:/Users/PHOENIXON/Documents/11121E-00_11122E-00_NorthVolt_5V100A_Formation_Channel/Channel Board/power_cpu1/dsp/c2000_sdk/v400/f2837xd/device/driverlib" --include_path="C:/Users/PHOENIXON/Documents/11121E-00_11122E-00_NorthVolt_5V100A_Formation_Channel/Channel Board/power_cpu1/dsp/c2000_sdk/v400/f2837xd/device/driverlib/inc" --include_path="C:/Users/PHOENIXON/Documents/11121E-00_11122E-00_NorthVolt_5V100A_Formation_Channel/Channel Board/power_cpu1/dsp/c2000_sdk/v400/f2837xd/libraries/flash_api/f2837xd/include" --include_path="C:/Users/PHOENIXON/Documents/11121E-00_11122E-00_NorthVolt_5V100A_Formation_Channel/Channel Board/power_cpu1/dsp/c2000_sdk/v400/f2837xd/libraries/calibration/hrpwm/f2837xd/include" --include_path="C:/Users/PHOENIXON/Documents/11121E-00_11122E-00_NorthVolt_5V100A_Formation_Channel/Channel Board/power_cpu1/dsp/c2000_sdk/v400/f2837xd/libraries/calibration/hrpwm/f2837xd/lib" --include_path="C:/Users/PHOENIXON/Documents/11121E-00_11122E-00_NorthVolt_5V100A_Formation_Channel/Channel Board/power_cpu1/dsp/c2000_sdk/v400/f2837xd/include" --include_path="C:/Users/PHOENIXON/Documents/11121E-00_11122E-00_NorthVolt_5V100A_Formation_Channel/Channel Board/power_cpu1/dsp/digital_power_sdk/v303/libraries/power_measurement/include" --include_path="C:/Users/PHOENIXON/Documents/11121E-00_11122E-00_NorthVolt_5V100A_Formation_Channel/Channel Board/power_cpu1/dsp/digital_power_sdk/v303/libraries/spll/include" --include_path="C:/Users/PHOENIXON/Documents/11121E-00_11122E-00_NorthVolt_5V100A_Formation_Channel/Channel Board/power_cpu1/dsp/digital_power_sdk/v303/libraries/transforms" --include_path="C:/Users/PHOENIXON/Documents/11121E-00_11122E-00_NorthVolt_5V100A_Formation_Channel/Channel Board/power_cpu1/dsp/digital_power_sdk/v303/libraries/utilities/rampgen/include" --include_path="C:/Users/PHOENIXON/Documents/11121E-00_11122E-00_NorthVolt_5V100A_Formation_Channel/Channel Board/power_cpu1/dsp/digital_power_sdk/v303/libraries/utilities/emavg/include" --include_path="C:/Users/PHOENIXON/Documents/11121E-00_11122E-00_NorthVolt_5V100A_Formation_Channel/Channel Board/power_cpu1/phoenixon/app_formation/app" --include_path="C:/Users/PHOENIXON/Documents/11121E-00_11122E-00_NorthVolt_5V100A_Formation_Channel/Channel Board/power_cpu1/phoenixon/app_formation/app/control" --include_path="C:/Users/PHOENIXON/Documents/11121E-00_11122E-00_NorthVolt_5V100A_Formation_Channel/Channel Board/power_cpu1/phoenixon/app_formation/app/comm" --include_path="C:/Users/PHOENIXON/Documents/11121E-00_11122E-00_NorthVolt_5V100A_Formation_Channel/Channel Board/power_cpu1/phoenixon/app_formation/app/test" --include_path="C:/Users/PHOENIXON/Documents/11121E-00_11122E-00_NorthVolt_5V100A_Formation_Channel/Channel Board/power_cpu1/phoenixon/app_formation/app/tick" --include_path="C:/Users/PHOENIXON/Documents/11121E-00_11122E-00_NorthVolt_5V100A_Formation_Channel/Channel Board/power_cpu1/phoenixon/app_formation/app/safety" --include_path="C:/Users/PHOENIXON/Documents/11121E-00_11122E-00_NorthVolt_5V100A_Formation_Channel/Channel Board/power_cpu1/phoenixon/app_formation/app/sense" --include_path="C:/Users/PHOENIXON/Documents/11121E-00_11122E-00_NorthVolt_5V100A_Formation_Channel/Channel Board/power_cpu1/phoenixon/app_formation/app/calibration" --include_path="C:/Users/PHOENIXON/Documents/11121E-00_11122E-00_NorthVolt_5V100A_Formation_Channel/Channel Board/power_cpu1/phoenixon/app_formation/bsp" --include_path="C:/Users/PHOENIXON/Documents/11121E-00_11122E-00_NorthVolt_5V100A_Formation_Channel/Channel Board/power_cpu1/phoenixon/app_formation/drv" --include_path="C:/Users/PHOENIXON/Documents/11121E-00_11122E-00_NorthVolt_5V100A_Formation_Channel/Channel Board/power_cpu1/phoenixon/utils" --advice:performance=all --define=ccs_c2k --define=_STANDALONE --define=DEBUG --define=_28377D --define=_DUAL_HEADERS --define=_FLASH --define=CPU1 -g --diag_suppress=10063 --diag_warning=225 --diag_wrap=off --display_error_number --abi=coffabi --preproc_with_compile --preproc_dependency="phoenixon/utils/$(basename $(<F)).d_raw" --obj_directory="phoenixon/utils" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


