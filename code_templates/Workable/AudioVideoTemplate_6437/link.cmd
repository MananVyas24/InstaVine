/*
 *  Copyright 2001 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 */
/*
 *  ======== hello.cmd ========
 *
 */



-l rts64plus.lib

-stack          0x00001000      /* Stack Size */
-heap           0x00001000      /* Heap Size */

MEMORY
{   
    BOOT_RAM:   o = 0x10800000  l = 0x00000400 
    INT:        o = 0x10800400  l = 0x00000400         
    L2RAM:      o = 0x10800800  l = 0x00020000
    DDR2:       o = 0x80000000  l = 0x10000000
}

 

SECTIONS
{
    .boot_load  >       BOOT_RAM 
    .bss        >   L2RAM
    .cinit      >   L2RAM
    .cio        >   L2RAM
    .const      >   L2RAM
    .data       >   L2RAM
    .far        >   L2RAM
    .stack      >   L2RAM
    .switch     >   L2RAM
    .sysmem     >   L2RAM
    .text       >   L2RAM
	.vectors > INT
    .ddr2       >   DDR2
}
