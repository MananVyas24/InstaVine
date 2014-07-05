/* ======================================================================== */
/*            Copyright (C) 2000 - 2009 ND-Tech. Co., Ltd.                  */
/*                          All Rights Reserved.                            */
/* ======================================================================== */
/* Project  : Cam2lcd3_EDMA                     Created  : 31-AUG-2009      */
/* File     : vm3224k.h                         Contact  : info@nd-tech.com */
/* ======================================================================== */
/* You are free to use or modify this code to the following restrictions:   */
/*          Acknowledge ND Tech. Co. Ltd.                                   */
/*          Or, put "Parts of code by ND Tech. Co., Ltd."                   */
/*          Or, leave this header as it is.                                 */
/* in somewhere in your code.                                               */
/* ======================================================================== */

#ifndef _VM3224K_H_
#define _VM3224K_H_

#define VM3224CNTL *(volatile short *)(0xa0000000) // VM3224 Control register
#define VM3224DATA *(volatile short *)(0xa0000004) // VM3224 Data register
#define VM3224ADDL *(volatile short *)(0xa0000008) // VM3224 Low address register
#define VM3224ADDH *(volatile short *)(0xa000000C) // VM3224 High address register

void 	i2c_init(short *i2csda,int sdabit,
                 short *i2cscl,int sclbit,
              	 int   devadd, int delay);
              
int 	i2c_Write(int add,int dat);
void	vm3224init(void);
void	vm3224bl(int n);
void	vm3224rate(int n);

#endif
