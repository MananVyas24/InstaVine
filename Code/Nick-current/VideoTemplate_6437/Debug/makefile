################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

EMPTY := 
SPACE := $(EMPTY) $(EMPTY)
CG_TOOL_ROOT := C:/Program Files (x86)/Texas Instruments/ccsv4/tools/compiler/c6000

ORDERED_OBJS += \
$(GEN_CMDS_QUOTED) \
"./ycbcr_rgb_wb.obj" \
"./vignette_quarter.obj" \
"./video_loopback_test.obj" \
"./tvp5146.obj" \
"./sepia.obj" \
"./focus_gray.obj" \
"./edge_detection_half.obj" \
"./diffuse.obj" \
"./color_temp.obj" \
"./White_balance.obj" \
"./Emboss_half.obj" \
"./ComicBook_half.obj" \
-l"libc.a" \
-l"C:\Program Files (x86)\Texas Instruments\ccsv4\tools\compiler\c6000\lib\rts64plus.lib" \
-l"C:\c64plus-imglib_2_02_00_00\lib\target\imglib2_elf.lib" \
"../evmdm6437bsl.lib" \
"../linker.cmd" \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: VideoTemplate_6437.out

# Tool invocations
VideoTemplate_6437.out: $(OBJS) $(CMD_SRCS) $(LIB_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: Linker'
	"C:/Program Files (x86)/Texas Instruments/ccsv4/tools/compiler/c6000/bin/cl6x" -mv6400+ --symdebug:none -O3 -ms3 --diag_warning=225 --abi=coffabi --opt_for_speed=5 -z -m"VideoTemplate_6437.map" --stack_size=800 --heap_size=800 --warn_sections -i"C:/Program Files (x86)/Texas Instruments/ccsv4/tools/compiler/c6000/lib" -i"C:/Program Files (x86)/Texas Instruments/ccsv4/tools/compiler/c6000/include" --reread_libs --rom_model -o "VideoTemplate_6437.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(C6000_EXECUTABLE_OUTPUTS__QTD) "VideoTemplate_6437.out"
	-$(RM) ".\ComicBook_half.obj" ".\Emboss_half.obj" ".\White_balance.obj" ".\color_temp.obj" ".\diffuse.obj" ".\edge_detection_half.obj" ".\focus_gray.obj" ".\sepia.obj" ".\tvp5146.obj" ".\video_loopback_test.obj" ".\vignette_quarter.obj" ".\ycbcr_rgb_wb.obj" 
	-$(RM) ".\ComicBook_half.pp" ".\Emboss_half.pp" ".\White_balance.pp" ".\color_temp.pp" ".\diffuse.pp" ".\edge_detection_half.pp" ".\focus_gray.pp" ".\sepia.pp" ".\tvp5146.pp" ".\video_loopback_test.pp" ".\vignette_quarter.pp" ".\ycbcr_rgb_wb.pp" 
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
