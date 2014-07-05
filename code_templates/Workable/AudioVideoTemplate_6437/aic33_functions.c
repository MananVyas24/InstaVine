#include "evmdm6437_aic33.h"
#include "aic33_functions.h"

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  AIC33 Tone                                                              *
 *      Output a 1 kHz tone through the HEADPHONE jacks             *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 aic33_tone_headphone( )
{
    Int16 msec, sec;
    Int16 sample;
    AIC33_CodecHandle aic33handle;

    /* Grab AIC33 handle */
    aic33handle = EVMDM6437_AIC33_openCodec( AIC33_MCBSP_ID, &aic33config_headphone );

    /* Play Tone */
    for ( sec = 0 ; sec < 5 ; sec++ )
    {
        for ( msec = 0 ; msec < 1000 ; msec++ )
        {
            for ( sample = 0 ; sample < 48 ; sample++ )
            {
                while ( ! EVMDM6437_AIC33_write16( aic33handle, sinetable[sample] ) );
            }
        }
    }

    EVMDM6437_AIC33_closeCodec( aic33handle );
    return 0;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  AIC33 Tone                                                              *
 *      Output a 1 kHz tone through the LINEOUT jacks             *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 aic33_tone_lineout( )
{
    Int16 msec, sec;
    Int16 sample;
    AIC33_CodecHandle aic33handle;

    /* Grab AIC33 handle */
    aic33handle = EVMDM6437_AIC33_openCodec( AIC33_MCBSP_ID, &aic33config_lineout );

    /* Play Tone */
    for ( sec = 0 ; sec < 5 ; sec++ )
    {
        for ( msec = 0 ; msec < 1000 ; msec++ )
        {
            for ( sample = 0 ; sample < 48 ; sample++ )
            {
                while ( ! EVMDM6437_AIC33_write16( aic33handle, sinetable[sample] ) );
            }
        }
    }

    EVMDM6437_AIC33_closeCodec( aic33handle );
    return 0;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  AIC33 Loopback                                                          *
 *      Audio loopback from LINEIN to HEADPHONE/LINEOUT jacks               *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 aic33_loop_linein_loopback( )
{
    Int16 msec, sec;
    Int16 sample;
    Int32 sample_data = 0;
    AIC33_CodecHandle aic33handle;

	Int16 sample_data_16 = 0;

    /* Grab AIC33 handle */
    aic33handle = EVMDM6437_AIC33_openCodec( AIC33_MCBSP_ID, &aic33config_linein_loopback );

    /* Play Loop */
    for ( sec = 0 ; sec < 5 ; sec++ )
    {
        /*
         *  For each second there are 48000 left/right samples
         */
        for ( msec = 0 ; msec < 1000 ; msec++ )
        {
            /*
             *  For each millisecond there are 48 left/right samples
             */
            for ( sample = 0 ; sample < 48 ; sample++ )
            {
                /* Read then write the right sample */
                while ( ! EVMDM6437_AIC33_read32( aic33handle, &sample_data ) );
                while ( ! EVMDM6437_AIC33_write32( aic33handle, sample_data ) );
            }
        }
    }

    EVMDM6437_AIC33_closeCodec( aic33handle );
    return 0;
  
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  AIC33 Loopback                                                          *
 *      Audio loopback from MICIN to HEADPHONE/LINEOUT jacks                *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 aic33_loop_micin_loopback( )
{
    Int16 msec, sec;
    Int16 sample;
    Int32 sample_data = 0;
    AIC33_CodecHandle aic33handle;

    /* Grab AIC33 handle */
    aic33handle = EVMDM6437_AIC33_openCodec( AIC33_MCBSP_ID, &aic33config_micin_loopback );

    /* Play Loop */
    for ( sec = 0 ; sec < 5 ; sec++ )
    {
        /*
         *  For each second there are 48000 left/right samples
         */
        for ( msec = 0 ; msec < 1000 ; msec++ )
        {
            /*
             *  For each millisecond there are 48 left/right samples
             */
            for ( sample = 0 ; sample < 48 ; sample++ )
            {
                while ( ! EVMDM6437_AIC33_read32( aic33handle, &sample_data ) );
                while ( ! EVMDM6437_AIC33_write32( aic33handle, sample_data ) );
            }
        }
    }

    EVMDM6437_AIC33_closeCodec( aic33handle );
    return 0;
}

