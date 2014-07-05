#
_XDCBUILDCOUNT = 
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = C:/PROGRA~2/TEXASI~1/bios_6_32_02_39/packages;C:/PROGRA~2/TEXASI~1/ipc_1_23_02_27/packages;C:/Users/Admin/Documents/workspace/Video_BIOS_template/.config
override XDCROOT = C:/PROGRA~2/TEXASI~1/xdctools_3_22_01_21
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = C:/PROGRA~2/TEXASI~1/bios_6_32_02_39/packages;C:/PROGRA~2/TEXASI~1/ipc_1_23_02_27/packages;C:/Users/Admin/Documents/workspace/Video_BIOS_template/.config;C:/PROGRA~2/TEXASI~1/xdctools_3_22_01_21/packages;..
HOSTOS = Windows
endif
