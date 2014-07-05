/*
 *  Copyright 2006 by Spectrum Digital Incorporated.
 *  All rights reserved. Property of Spectrum Digital Incorporated.
 */

/*
 *  TVP5146 Video Decoder
 *
 */


#include "evmdm6437_i2c.h"

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  TVP5146 Definitions                                                     *
 *                                                                          *
 * ------------------------------------------------------------------------ */
#define TVP5146_I2C_ADDR    0x5D

#define NTSC                1
#define PAL                 0

#define SVIDEO_IN           1
#define COMPONENT_IN        0

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  Prototypes                                                              *
 *                                                                          *
 * ------------------------------------------------------------------------ */
void tvp5146_init( Uint32 ntsc_pal_mode, Uint32 input_mode );
void tvp5146_rset( Uint8 regnum, Uint8 regval );
Uint8 tvp5146_rget( Uint8 regnum );
