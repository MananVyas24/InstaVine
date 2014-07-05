################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
LIB_SRCS += \
../evmdm6437bsl.lib 

CFG_SRCS += \
../app.cfg 

C_SRCS += \
../main.c \
../tahomabold_20.c \
../text_functions.c \
../tvp5146.c \
../video_loopback_test.c 

GEN_SRCS += \
./configPkg/compiler.opt \
./configPkg/linker.cmd 

GEN_CMDS += \
./configPkg/linker.cmd 

OBJS += \
./main.obj \
./tahomabold_20.obj \
./text_functions.obj \
./tvp5146.obj \
./video_loopback_test.obj 

C_DEPS += \
./main.pp \
./tahomabold_20.pp \
./text_functions.pp \
./tvp5146.pp \
./video_loopback_test.pp 

GEN_MISC_DIRS += \
./configPkg/ 

GEN_OPTS += \
./configPkg/compiler.opt 

OBJS__QTD += \
".\main.obj" \
".\tahomabold_20.obj" \
".\text_functions.obj" \
".\tvp5146.obj" \
".\video_loopback_test.obj" 

GEN_SRCS__QTD += \
".\configPkg\compiler.opt" \
".\configPkg\linker.cmd" 

C_DEPS__QTD += \
".\main.pp" \
".\tahomabold_20.pp" \
".\text_functions.pp" \
".\tvp5146.pp" \
".\video_loopback_test.pp" 

GEN_MISC_DIRS__QTD += \
".\configPkg\" 

CFG_SRCS_QUOTED += \
"../app.cfg" 

GEN_OPTS_QUOTED += \
--cmd_file="./configPkg/compiler.opt" 

GEN_CMDS_QUOTED += \
-l"./configPkg/linker.cmd" 

C_SRCS_QUOTED += \
"../main.c" \
"../tahomabold_20.c" \
"../text_functions.c" \
"../tvp5146.c" \
"../video_loopback_test.c" 


