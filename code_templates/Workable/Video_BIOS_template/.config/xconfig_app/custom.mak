## THIS IS A GENERATED FILE -- DO NOT EDIT
.configuro: .libraries,64P linker.cmd \
  package/cfg/app_p64P.o64P \

linker.cmd: package/cfg/app_p64P.xdl
	$(SED) 's"^\"\(package/cfg/app_p64Pcfg.cmd\)\"$""\"C:/Users/Admin/Documents/workspace/Video_BIOS_template/.config/xconfig_app/\1\""' package/cfg/app_p64P.xdl > $@
