################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
LIB_SRCS += \
../evmdm6437bsl.lib 

C_SRCS += \
../aic33_functions.c \
../main.c \
../tvp5146.c \
../video_loopback_test.c 

ASM_SRCS += \
../boot.asm \
../vecs_timer.asm 

CMD_SRCS += \
../link.cmd 

ASM_DEPS += \
./boot.pp \
./vecs_timer.pp 

OBJS += \
./aic33_functions.obj \
./boot.obj \
./main.obj \
./tvp5146.obj \
./vecs_timer.obj \
./video_loopback_test.obj 

C_DEPS += \
./aic33_functions.pp \
./main.pp \
./tvp5146.pp \
./video_loopback_test.pp 

OBJS__QTD += \
".\aic33_functions.obj" \
".\boot.obj" \
".\main.obj" \
".\tvp5146.obj" \
".\vecs_timer.obj" \
".\video_loopback_test.obj" 

ASM_DEPS__QTD += \
".\boot.pp" \
".\vecs_timer.pp" 

C_DEPS__QTD += \
".\aic33_functions.pp" \
".\main.pp" \
".\tvp5146.pp" \
".\video_loopback_test.pp" 

C_SRCS_QUOTED += \
"../aic33_functions.c" \
"../main.c" \
"../tvp5146.c" \
"../video_loopback_test.c" 

ASM_SRCS_QUOTED += \
"../boot.asm" \
"../vecs_timer.asm" 


