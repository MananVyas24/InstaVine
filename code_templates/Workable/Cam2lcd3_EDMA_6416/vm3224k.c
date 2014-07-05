/* ======================================================================== */
/*            Copyright (C) 2000 - 2009 ND-Tech. Co., Ltd.                  */
/*                          All Rights Reserved.                            */
/* ======================================================================== */
/* Project  : Cam2lcd3_EDMA                     Created  : 31-AUG-2009      */
/* File     : vm3224k.c                         Contact  : info@nd-tech.com */
/* ======================================================================== */
/* You are free to use or modify this code to the following restrictions:   */
/*          Acknowledge ND Tech. Co. Ltd.                                   */
/*          Or, put "Parts of code by ND Tech. Co., Ltd."                   */
/*          Or, leave this header as it is.                                 */
/* in somewhere in your code.                                               */
/* ======================================================================== */

#include	"vm3224k.h"

static short	*I2CSDA,*I2CSCL;
static short	DEVADD,I2CDLY;
static short	SDAHI,SCLHI,SDALO,SCLLO;

// NTSC
static char	saa7113[] = {
//	0x01, 23,
 	0x08,0xc3,0x33,0x00,0x00,0xe9,0x0d,
	0xf8,0x03,0x80,0x47,0x40,0x00,0x01,0x2a,
	0x00,0x0c,0xb1,0x00,0x00,0x00,0x00,0x00,
//	0x40, 6,
	0xe2,0x00,0x54,0x0a,0x83,0x00
};
/* PAL
static char saa7113[] = {
//	0x01, 0x23,
	0x08,0xc3,0x33,0x00,0x00,0xe9,0x0d,
	0xb8,0x03,0x80,0x47,0x40,0x00,0x01,0x2a,
	0x00,0x0c,0xb1,0x00,0x00,0x00,0x00,0x00,
//  0x40,
	0x62,0x00,0x54,0x0a,0x83,0x00
};*/


void i2c_init(	short *i2csda,int sdabit,
				short *i2cscl,int sclbit,
              	int devadd,int delay)
{	
	I2CDLY = delay;
	DEVADD = devadd&0x0fffe;
	I2CSDA = i2csda;
	I2CSCL = i2cscl;
	
	SDAHI = (1<<sdabit);
	SDALO = ~(1<<sdabit);
	SCLHI = (1<<sclbit);
	SCLLO = ~(1<<sclbit);
	*I2CSCL |= SCLHI;
	*I2CSDA |= SDAHI;
}

static void delay(int delay)
{
	while(delay>0) delay -= 5;
}

static void	i2c_start()
{
	*I2CSDA &= SDALO;
	delay(I2CDLY);
	*I2CSCL &= SCLLO;
}

static void	i2c_stop()
{
	*I2CSDA &= SDALO;
	delay(I2CDLY);
	*I2CSCL |= SCLHI;
	delay(I2CDLY);
	*I2CSDA |= SDAHI;
}

static void i2c_xbit(int bit)
{	
	if (bit) *I2CSDA |= SDAHI;
	else *I2CSDA &= SDALO;
	delay(I2CDLY);
	*I2CSCL |= SCLHI; delay(I2CDLY<<1);
	*I2CSCL &= SCLLO;
	return;
}

static int i2c_rbit(void)
{	
	int	rtn;
	
	*I2CSDA |= SDAHI; delay(I2CDLY);
	*I2CSCL |= SCLHI; delay(I2CDLY);
	rtn = *I2CSDA & SDAHI;
	*I2CSCL &= SCLLO; delay(I2CDLY);
	if (rtn) return 1;
	else return 0;
}

static void i2c_xbyte(int byte)
{	
	int	i;
	for (i=7;i>=0;i--) i2c_xbit((byte>>i)&1);
	return;
}

int i2c_Write(int add,int dat)
{
	int rtn;	

	i2c_start();
	i2c_xbyte(DEVADD);i2c_rbit();
	i2c_xbyte(add);i2c_rbit();
	i2c_xbyte(dat); 
	rtn = i2c_rbit();
	i2c_stop();	
	delay(I2CDLY<<2);
	return rtn;
}

void	vm3224init(void)
{
	int		i;
	
	VM3224CNTL = 0x000d;		// Reset, Back Light off
	for (i=0;i<0x200000;i++);	// Reset delay
	VM3224CNTL = 0xff3c;		// Reset release, Back Light maximum
	for (i=0;i<0x200000;i++);	// Reset release delay
	i2c_init((short *)&VM3224CNTL,2,(short *)&VM3224CNTL,3,0x48,600);

	for(i=0;i<23;i++) i2c_Write(i+1,saa7113[i]);
	for(i=0;i<6;i++) i2c_Write(i+0x40,saa7113[i+23]);
}

void	vm3224rate(int n)
{
	// n = 0 : freeze
	// n = 1 : 7.5 fps for NTSC, 6.25 fps for PA
	// n = 2 : 15 fps for NTSC, 12.5 fps for PAL
	// n = 3 : 30 fps for NTSC, 25 fps for PAL
	VM3224CNTL = ((VM3224CNTL&0xffcf)|((n&3)<<4));
}

void	vm3224bl(int n)
{
	// n : 0~15 : backlight off ~ maximum (0~15)
	VM3224CNTL = ((VM3224CNTL&0x0fff)|((n&0x0f)<<12));
}
