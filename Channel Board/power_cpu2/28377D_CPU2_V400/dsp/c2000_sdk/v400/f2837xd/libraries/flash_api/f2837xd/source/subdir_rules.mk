################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
dsp/c2000_sdk/v400/f2837xd/libraries/flash_api/f2837xd/source/Fapi_UserDefinedFunctions.obj: D:/1.Phoenixon_Work/06.Ccs_Workspace/04.Fomation5V100A/4_Channel_card/Formation_Channel/power_cpu1/dsp/c2000_sdk/v400/f2837xd/libraries/flash_api/f2837xd/source/Fapi_UserDefinedFunctions.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs1011/ccs/tools/compiler/ti-cgt-c2000_21.6.0.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 -Ooff --fp_mode=strict --include_path="D:/1.Phoenixon_Work/06.Ccs_Workspace/04.Fomation5V100A/4_Channel_card/Formation_Channel/power_cpu1/phoenixon/app_formation/app" --include_path="D:/1.Phoenixon_Work/06.Ccs_Workspace/04.Fomation5V100A/4_Channel_card/Formation_Channel/power_cpu1/phoenixon/app_formation/app/comm" --include_path="D:/1.Phoenixon_Work/06.Ccs_Workspace/04.Fomation5V100A/4_Channel_card/Formation_Channel/power_cpu1/phoenixon/app_formation/app/control" --include_path="D:/1.Phoenixon_Work/06.Ccs_Workspace/04.Fomation5V100A/4_Channel_card/Formation_Channel/power_cpu1/phoenixon/app_formation/app/sense" --include_path="D:/1.Phoenixon_Work/06.Ccs_Workspace/04.Fomation5V100A/4_Channel_card/Formation_Channel/power_cpu1/phoenixon/app_formation/app/calibration" --include_path="D:/1.Phoenixon_Work/06.Ccs_Workspace/04.Fomation5V100A/4_Channel_card/Formation_Channel/power_cpu1/phoenixon/app_formation/app/tick" --include_path="D:/1.Phoenixon_Work/06.Ccs_Workspace/04.Fomation5V100A/4_Channel_card/Formation_Channel/power_cpu1/phoenixon/app_formation/app/safety" --include_path="D:/1.Phoenixon_Work/06.Ccs_Workspace/04.Fomation5V100A/4_Channel_card/Formation_Channel/power_cpu1/phoenixon/app_formation/bsp" --include_path="D:/1.Phoenixon_Work/06.Ccs_Workspace/04.Fomation5V100A/4_Channel_card/Formation_Channel/power_cpu1/phoenixon/app_formation/drv" --include_path="D:/1.Phoenixon_Work/06.Ccs_Workspace/04.Fomation5V100A/4_Channel_card/Formation_Channel/power_cpu1/phoenixon/utils" --include_path="C:/ti/ccs1011/ccs/tools/compiler/ti-cgt-c2000_21.6.0.LTS/include" --include_path="D:/1.Phoenixon_Work/06.Ccs_Workspace/04.Fomation5V100A/4_Channel_card/Formation_Channel/power_cpu2" --include_path="D:/1.Phoenixon_Work/06.Ccs_Workspace/04.Fomation5V100A/4_Channel_card/Formation_Channel/power_cpu1/dsp/c2000_sdk/v400/f2837xd" --include_path="D:/1.Phoenixon_Work/06.Ccs_Workspace/04.Fomation5V100A/4_Channel_card/Formation_Channel/power_cpu1/dsp/c2000_sdk/v400/f2837xd/common" --include_path="D:/1.Phoenixon_Work/06.Ccs_Workspace/04.Fomation5V100A/4_Channel_card/Formation_Channel/power_cpu1/dsp/c2000_sdk/v400/f2837xd/device" --include_path="D:/1.Phoenixon_Work/06.Ccs_Workspace/04.Fomation5V100A/4_Channel_card/Formation_Channel/power_cpu1/dsp/c2000_sdk/v400/f2837xd/device/driverlib" --include_path="D:/1.Phoenixon_Work/06.Ccs_Workspace/04.Fomation5V100A/4_Channel_card/Formation_Channel/power_cpu1/dsp/c2000_sdk/v400/f2837xd/device/driverlib/inc" --include_path="D:/1.Phoenixon_Work/06.Ccs_Workspace/04.Fomation5V100A/4_Channel_card/Formation_Channel/power_cpu1/dsp/c2000_sdk/v400/f2837xd/include" --include_path="D:/1.Phoenixon_Work/06.Ccs_Workspace/04.Fomation5V100A/4_Channel_card/Formation_Channel/power_cpu1/dsp/c2000_sdk/v400/f2837xd/libraries/flash_api/f2837xd/include" --include_path="D:/1.Phoenixon_Work/06.Ccs_Workspace/04.Fomation5V100A/4_Channel_card/Formation_Channel/power_cpu1/dsp/c2000_sdk/v400/f2837xd/libraries/calibration/hrpwm/f2837xd/include" --include_path="D:/1.Phoenixon_Work/06.Ccs_Workspace/04.Fomation5V100A/4_Channel_card/Formation_Channel/power_cpu1/dsp/digital_power_sdk/v303/libraries/power_measurement/include" --include_path="D:/1.Phoenixon_Work/06.Ccs_Workspace/04.Fomation5V100A/4_Channel_card/Formation_Channel/power_cpu1/dsp/digital_power_sdk/v303/libraries/utilities/rampgen/include" --include_path="D:/1.Phoenixon_Work/06.Ccs_Workspace/04.Fomation5V100A/4_Channel_card/Formation_Channel/power_cpu1/dsp/digital_power_sdk/v303/libraries/utilities/emavg/include" --include_path="D:/1.Phoenixon_Work/06.Ccs_Workspace/04.Fomation5V100A/4_Channel_card/Formation_Channel/power_cpu1/dsp/digital_power_sdk/v303/libraries/spll/include" --include_path="D:/1.Phoenixon_Work/06.Ccs_Workspace/04.Fomation5V100A/4_Channel_card/Formation_Channel/power_cpu1/dsp/digital_power_sdk/v303/libraries/transforms" --advice:performance=all --define=ccs_c2k --define=_28377D --define=_DUAL_HEADERS --define=_FLASH --define=DEBUG --define=CPU2 --diag_suppress=10063 --diag_warning=225 --diag_wrap=off --display_error_number --abi=coffabi --preproc_with_compile --preproc_dependency="dsp/c2000_sdk/v400/f2837xd/libraries/flash_api/f2837xd/source/$(basename $(<F)).d_raw" --obj_directory="dsp/c2000_sdk/v400/f2837xd/libraries/flash_api/f2837xd/source" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


