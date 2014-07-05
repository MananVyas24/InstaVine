/*
 *  Copyright 2006 by Spectrum Digital Incorporated.
 *  All rights reserved. Property of Spectrum Digital Incorporated.
 */

/*
 *  TVP5146 Video Decoder
 *
 */

#include "tvp5146.h"

Uint8 rom_version;
Uint8 chipid_msb;
Uint8 chipid_lsb;

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  tvp5146_rset                                                            *
 *                                                                          *
 *      Set codec register regnum to value regval                           *
 *                                                                          *
 * ------------------------------------------------------------------------ */
void tvp5146_rset( Uint8 regnum, Uint8 regval )
{
    Uint8 cmd[2];
    cmd[0] = regnum;    // 8-bit Register Address
    cmd[1] = regval;    // 8-bit Register Data

    EVMDM6437_I2C_write( TVP5146_I2C_ADDR, cmd, 2 );
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  tvp5146_rget                                                            *
 *                                                                          *
 *      Return value of codec register regnum                               *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Uint8 tvp5146_rget( Uint8 regnum )
{
    Uint8 cmd[2];

    cmd[0] = regnum;    // 8-bit Register Address
    cmd[1] = 0;         // 8-bit Register Data

    EVMDM6437_I2C_write( TVP5146_I2C_ADDR, cmd, 1 );
    EVMDM6437_I2C_read ( TVP5146_I2C_ADDR, cmd, 1 );

    return cmd[0];
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  tvp5146_init( )                                                         *
 *                                                                          *
 *      Initialize the TVP5146                                              *
 *                                                                          *
 * ------------------------------------------------------------------------ */
void tvp5146_init( Uint32 ntsc_pal_mode, Uint32 input_mode )
{
    rom_version = tvp5146_rget( 0x70 );
    chipid_msb  = tvp5146_rget( 0x80 );
    chipid_lsb  = tvp5146_rget( 0x81 );

    _waitusec( 1000 );                  // wait 1 msec

    if ( rom_version < 8 )
    {
        tvp5146_rset( 0xE8, 0x02 );     // Initalize TVP5146, must do after power on
        tvp5146_rset( 0xE9, 0x00 );     // Skip if using TVP5146-M2
        tvp5146_rset( 0xEA, 0x80 );
        tvp5146_rset( 0xE0, 0x01 );
        tvp5146_rset( 0xE8, 0x60 );
        tvp5146_rset( 0xE9, 0x00 );
        tvp5146_rset( 0xEA, 0xB0 );
        tvp5146_rset( 0xE0, 0x01 );
        tvp5146_rset( 0xE0, 0x00 );

        _waitusec( 1000 );              // wait 1 msec
    }

    switch( input_mode )
    {
        case SVIDEO_IN:
            tvp5146_rset( 0x00, 0x46 ); // Input Video: S-video: VI_2_C(Y) VI_1_C(C)
            break;

        case COMPONENT_IN:
            tvp5146_rset( 0x00, 0x05 ); // Input Video: CVBS : VI_2_B
            break;
    }

    switch( ntsc_pal_mode )
    {
        case NTSC:
            tvp5146_rset( 0x02, 0x01 );
            break;
        case PAL:
            tvp5146_rset( 0x02, 0x02 );
            break;
    }

    tvp5146_rset( 0x34, 0x11 );         // Enabling clock & Y/CB/CR input format

    _waitusec( 1000 );                  // wait 1 msec
}
