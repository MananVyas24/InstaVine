#ifndef AIC33_FUNCTIONS_H_
#define AIC33_FUNCTIONS_H_


#include "evmdm6437_aic33.h"
#include "aic33_configuration.h"

/* Pre-generated sine wave data, 16-bit signed samples */
static Int16 sinetable[48] = {
    0x0000, 0x10b4, 0x2120, 0x30fb, 0x3fff, 0x4dea, 0x5a81, 0x658b,
    0x6ed8, 0x763f, 0x7ba1, 0x7ee5, 0x7ffd, 0x7ee5, 0x7ba1, 0x76ef,
    0x6ed8, 0x658b, 0x5a81, 0x4dea, 0x3fff, 0x30fb, 0x2120, 0x10b4,
    0x0000, 0xef4c, 0xdee0, 0xcf06, 0xc002, 0xb216, 0xa57f, 0x9a75,
    0x9128, 0x89c1, 0x845f, 0x811b, 0x8002, 0x811b, 0x845f, 0x89c1,
    0x9128, 0x9a76, 0xa57f, 0xb216, 0xc002, 0xcf06, 0xdee0, 0xef4c
};

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  AIC33 Tone                                                              *
 *      Output a 1 kHz tone through the HEADPHONE/LINEOUT jacks             *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 aic33_tone_headphone( );

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  AIC33 Tone                                                              *
 *      Output a 1 kHz tone through the HEADPHONE/LINEOUT jacks             *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 aic33_tone_lineout( );

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  AIC33 Loopback                                                          *
 *      Audio loopback from LINEIN to HEADPHONE/LINEOUT jacks               *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 aic33_loop_linein_loopback( );

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  AIC33 Loopback                                                          *
 *      Audio loopback from MICIN to HEADPHONE/LINEOUT jacks                *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 aic33_loop_micin_loopback( );




#endif /*AIC33_FUNCTIONS_H_*/
