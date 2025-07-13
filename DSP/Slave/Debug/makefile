################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/ccs930/ccs/tools/compiler/ti-cgt-c2000_18.12.4.LTS

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./main.obj" \
"./src/AD2S.obj" \
"./src/DSP2833x_ADC_cal.obj" \
"./src/DSP2833x_Adc.obj" \
"./src/DSP2833x_CSMPasswords.obj" \
"./src/DSP2833x_CodeStartBranch.obj" \
"./src/DSP2833x_DBGIER.obj" \
"./src/DSP2833x_DMA.obj" \
"./src/DSP2833x_DefaultIsr.obj" \
"./src/DSP2833x_DisInt.obj" \
"./src/DSP2833x_ECan.obj" \
"./src/DSP2833x_ECap.obj" \
"./src/DSP2833x_EPwm.obj" \
"./src/DSP2833x_GlobalVariableDefs.obj" \
"./src/DSP2833x_Gpio.obj" \
"./src/DSP2833x_I2C.obj" \
"./src/DSP2833x_Mcbsp.obj" \
"./src/DSP2833x_MemCopy.obj" \
"./src/DSP2833x_PieCtrl.obj" \
"./src/DSP2833x_PieVect.obj" \
"./src/DSP2833x_Sci.obj" \
"./src/DSP2833x_Spi.obj" \
"./src/DSP2833x_SysCtrl.obj" \
"./src/DSP2833x_Xintf.obj" \
"./src/DSP2833x_usDelay.obj" \
"./src/SPI4.obj" \
"./src/User_Comm.obj" \
"./src/User_InitVar.obj" \
"./src/User_Isr.obj" \
"./src/User_Menu.obj" \
"./src/User_Modbus.obj" \
"./src/User_subFunc.obj" \
"./src/CANopen/CO_DS402.obj" \
"./src/CANopen/CO_OD.obj" \
"./src/CANopen/CO_driver.obj" \
"./src/CANopen/CO_stack.obj" \
"./src/CANopen/CO_user.obj" \
"./src/DPRAM/User_DPRAM.obj" \
"./src/DPRAM/User_DPRAM_OD.obj" \
"./src/Test/User_test.obj" \
"../DSP2833x_Headers_nonBIOS.cmd" \
"../F28335.cmd" \
"../lib/IQmath_fpu32.lib" \
"../lib/rts2800_fpu32_fast_supplement.lib" \
$(GEN_CMDS__FLAG) \
-llibc.a \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include lib/subdir_vars.mk
-include src/subdir_vars.mk
-include src/CANopen/subdir_vars.mk
-include src/DPRAM/subdir_vars.mk
-include src/Test/subdir_vars.mk
-include targetConfigs/subdir_vars.mk
-include subdir_rules.mk
-include lib/subdir_rules.mk
-include src/subdir_rules.mk
-include src/CANopen/subdir_rules.mk
-include src/DPRAM/subdir_rules.mk
-include src/Test/subdir_rules.mk
-include targetConfigs/subdir_rules.mk
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
SlaveOK.out \

EXE_OUTPUTS__QUOTED += \
"SlaveOK.out" \

BIN_OUTPUTS += \
SlaveOK.hex \

BIN_OUTPUTS__QUOTED += \
"SlaveOK.hex" \


# All Target
all: SlaveOK.out

# Tool invocations
SlaveOK.out: $(OBJS) $(CMD_SRCS) $(LIB_SRCS) $(GEN_CMDS)
	@echo 'Building target: "$@"'
	@echo 'Invoking: C2000 Linker'
	"C:/ti/ccs930/ccs/tools/compiler/ti-cgt-c2000_18.12.4.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 -g --diag_warning=225 --diag_wrap=off --display_error_number --abi=coffabi -z -m"SlaveOK.map" --stack_size=0x300 --warn_sections -i"C:/ti/ccs930/ccs/tools/compiler/ti-cgt-c2000_18.12.4.LTS/lib" -i"E:/WORK/BigMoto/STM CODE/0627MyPronetE/dsp/C1C1OK FOR bigmoto/Slave/include" -i"C:/ti/ccs930/ccs/tools/compiler/ti-cgt-c2000_18.12.4.LTS/include" --reread_libs --diag_wrap=off --display_error_number --xml_link_info="SlaveOK_linkInfo.xml" --rom_model -o "SlaveOK.out" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '

SlaveOK.hex: $(EXE_OUTPUTS)
	@echo 'Building files: $(strip $(EXE_OUTPUTS__QUOTED))'
	@echo 'Invoking: C2000 Hex Utility'
	"C:/ti/ccs930/ccs/tools/compiler/ti-cgt-c2000_18.12.4.LTS/bin/hex2000"  -o "SlaveOK.hex" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building: $(strip $(EXE_OUTPUTS__QUOTED))'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(BIN_OUTPUTS__QUOTED)$(EXE_OUTPUTS__QUOTED)
	-$(RM) "main.obj" "src\AD2S.obj" "src\DSP2833x_ADC_cal.obj" "src\DSP2833x_Adc.obj" "src\DSP2833x_CSMPasswords.obj" "src\DSP2833x_CodeStartBranch.obj" "src\DSP2833x_DBGIER.obj" "src\DSP2833x_DMA.obj" "src\DSP2833x_DefaultIsr.obj" "src\DSP2833x_DisInt.obj" "src\DSP2833x_ECan.obj" "src\DSP2833x_ECap.obj" "src\DSP2833x_EPwm.obj" "src\DSP2833x_GlobalVariableDefs.obj" "src\DSP2833x_Gpio.obj" "src\DSP2833x_I2C.obj" "src\DSP2833x_Mcbsp.obj" "src\DSP2833x_MemCopy.obj" "src\DSP2833x_PieCtrl.obj" "src\DSP2833x_PieVect.obj" "src\DSP2833x_Sci.obj" "src\DSP2833x_Spi.obj" "src\DSP2833x_SysCtrl.obj" "src\DSP2833x_Xintf.obj" "src\DSP2833x_usDelay.obj" "src\SPI4.obj" "src\User_Comm.obj" "src\User_InitVar.obj" "src\User_Isr.obj" "src\User_Menu.obj" "src\User_Modbus.obj" "src\User_subFunc.obj" "src\CANopen\CO_DS402.obj" "src\CANopen\CO_OD.obj" "src\CANopen\CO_driver.obj" "src\CANopen\CO_stack.obj" "src\CANopen\CO_user.obj" "src\DPRAM\User_DPRAM.obj" "src\DPRAM\User_DPRAM_OD.obj" "src\Test\User_test.obj" 
	-$(RM) "main.d" "src\AD2S.d" "src\DSP2833x_Adc.d" "src\DSP2833x_DMA.d" "src\DSP2833x_DefaultIsr.d" "src\DSP2833x_ECan.d" "src\DSP2833x_ECap.d" "src\DSP2833x_EPwm.d" "src\DSP2833x_GlobalVariableDefs.d" "src\DSP2833x_Gpio.d" "src\DSP2833x_I2C.d" "src\DSP2833x_Mcbsp.d" "src\DSP2833x_MemCopy.d" "src\DSP2833x_PieCtrl.d" "src\DSP2833x_PieVect.d" "src\DSP2833x_Sci.d" "src\DSP2833x_Spi.d" "src\DSP2833x_SysCtrl.d" "src\DSP2833x_Xintf.d" "src\SPI4.d" "src\User_Comm.d" "src\User_InitVar.d" "src\User_Isr.d" "src\User_Menu.d" "src\User_Modbus.d" "src\User_subFunc.d" "src\CANopen\CO_DS402.d" "src\CANopen\CO_OD.d" "src\CANopen\CO_driver.d" "src\CANopen\CO_stack.d" "src\CANopen\CO_user.d" "src\DPRAM\User_DPRAM.d" "src\DPRAM\User_DPRAM_OD.d" "src\Test\User_test.d" 
	-$(RM) "src\DSP2833x_ADC_cal.d" "src\DSP2833x_CSMPasswords.d" "src\DSP2833x_CodeStartBranch.d" "src\DSP2833x_DBGIER.d" "src\DSP2833x_DisInt.d" "src\DSP2833x_usDelay.d" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

