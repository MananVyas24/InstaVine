################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
main.obj: ../main.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"C:/Program Files (x86)/Texas Instruments/ccsv4/tools/compiler/c6000/bin/cl6x" -mv6400 -g --define="_DEBUG" --include_path="C:/Program Files (x86)/Texas Instruments/ccsv4/tools/compiler/c6000/include" --include_path="C:/Program Files (x86)/Texas Instruments/xdais_7_10_00_06/packages/ti/xdais" --include_path="/include" --include_path="C:/Program Files (x86)/Texas Instruments/ccsv4/C6000/csl/include" --include_path="C:/Program Files (x86)/Texas Instruments/ccsv4/C6000/dsk6416/include" --include_path="C:/Program Files (x86)/Texas Instruments/ccsv4/IMGLIB/C6400/imglib/include" --diag_warning=225 --abi=coffabi --preproc_with_compile --preproc_dependency="main.pp" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '

vm3224k.obj: ../vm3224k.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"C:/Program Files (x86)/Texas Instruments/ccsv4/tools/compiler/c6000/bin/cl6x" -mv6400 -g --define="_DEBUG" --include_path="C:/Program Files (x86)/Texas Instruments/ccsv4/tools/compiler/c6000/include" --include_path="C:/Program Files (x86)/Texas Instruments/xdais_7_10_00_06/packages/ti/xdais" --include_path="/include" --include_path="C:/Program Files (x86)/Texas Instruments/ccsv4/C6000/csl/include" --include_path="C:/Program Files (x86)/Texas Instruments/ccsv4/C6000/dsk6416/include" --include_path="C:/Program Files (x86)/Texas Instruments/ccsv4/IMGLIB/C6400/imglib/include" --diag_warning=225 --abi=coffabi --preproc_with_compile --preproc_dependency="vm3224k.pp" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '


