-q 
-cr 
-m main.map 
-o main.out 
-stack 0x4000

/* Memory for TMS320C6416 */
MEMORY   { IDRAM  : origin = 0x00000000,len = 0x00100000 }
		   
SECTIONS { .vectors:    {} > 0h
           .text:       {} > IDRAM
           .const:      {} > IDRAM
           .cinit:      {} > IDRAM
           .bss:        {} > IDRAM
           .sysmem:     {} > IDRAM
           .cio:        {} > IDRAM
           .far:        {} > IDRAM
           .stack:      {} > IDRAM }






