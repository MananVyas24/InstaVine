;* ======================================================================== *;
;*            Copyright (C) 2000 - 2009 ND-Tech. Co., Ltd.                  *;
;*                          All Rights Reserved.                            *;
;* ======================================================================== *;
;* Project  : Cam2lcd3_EDMA                     Created  : 31-AUG-2009      *;
;* File     : start.asm                         Contact  : info@nd-tech.com *;
;* ======================================================================== *;
;* You are free to use or modify this code to the following restrictions:   *;
;*          Acknowledge ND Tech. Co. Ltd.                                   *;
;*          Or, put "Parts of code by ND Tech. Co., Ltd."                   *;
;*          Or, leave this header as it is.                                 *;
;* in somewhere in your code.                                               *;
;* ======================================================================== *;
			.global	_c_int00

			.sect	.vectors
reset:		B.S2	_c_int00
			NOP		5
			.space	0x18
NMI			B.S2	NRP
			NOP		5
			.space	0x58
INT4		B.S2	IRP
			NOP		5
			.space	0x18 
INT5		B.S2	IRP
			NOP		5
			.space	0x18 
INT6		B.S2	IRP
			NOP		5
			.space	0x18 
INT7		B.S2	IRP
			NOP		5
			.space	0x18 
INT8		B.S2	IRP
			NOP		5
			.space	0x18 
INT9		B.S2	IRP
			NOP		5
			.space	0x18 
INT10		B.S2	IRP
			NOP		5
			.space	0x18 
INT11		B.S2	IRP
			NOP		5
			.space	0x18 
INT12		B.S2	IRP
			NOP		5
			.space	0x18 
INT13		B.S2	IRP
			NOP		5
			.space	0x18 
INT14		B.S2	IRP
			NOP		5
			.space	0x18 
INT15		B.S2	IRP
			NOP		5
			.space	0x18 

			.end
