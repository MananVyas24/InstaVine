/*
* Video Loopback Test
*
*/

#include "stdio.h"
#include "evmdm6437_dip.h"
#include "tvp5146.h"

#define NTSC 1
#define PAL 0

#define LOOPBACK 0

#define SVIDEO_OUT 1
#define COMPOSITE_OUT 0

#define mem_buffer_out 0x80000000 //Start address for the buffer_out
#define mem_buffer_in 0x800A8C00 // Start address for the buffer_in (size: 720 * 480 / 2 * 4)
/* ------------------------------------------------------------------------ *
* *
* vpfe_init( ntsc/pal ) *
* *
* ------------------------------------------------------------------------ */

static void vpfe_init( Uint32 ntsc_pal_mode, Uint32 video_buffer )
{
	//Uint32 video_buffer = mem_buffer_in;
	Uint32 width;
	Uint32 height;

	//Definition of the image in number of pixels
	if ( ntsc_pal_mode == NTSC ){
		width = 720;
		height = 480;
	} else {
		width = 720; //Number of pixels on a line
		height = 576; //Number of lines
	}

	VPFE_CCDC_PCR = 0x00000001; //CCD controller enabled
	VPFE_CCDC_SYN_MODE = 0x00032F85; //interlaced, with VD polarity as
									 //negative
	VPFE_CCDC_HD_VD_WID = 0;
	VPFE_CCDC_PIX_LINES = 0x02CF020D; //Defined the pixels per line and
									  //the half lines per frame
	/*
	* sph = 1, nph = 1440, according to page 32-33 of the CCDC spec
	* for BT.656 mode, this setting captures only the 720x480 of the
	* active NTSV video window
	*/
	VPFE_CCDC_HORZ_INFO = width << 1; // Horizontal pixels that are
									  //output to the SDRAM
	VPFE_CCDC_HSIZE_OFF = width << 1; // Offset for each output line in SDRAM
	VPFE_CCDC_VERT_START = 0; // Vertical start line
	VPFE_CCDC_VERT_LINES = height >> 1; // Vertical lines that are output
										//to the SDRAM
	VPFE_CCDC_CULLING = 0xFFFF00FF; // Disable culling

	/*
	* Interleave the two fields
	*/
	VPFE_CCDC_SDOFST = 0x00000249; //How the values are save on the
								   //memory
	VPFE_CCDC_SDR_ADDR = video_buffer; //Start address save the input
									   //values
	VPFE_CCDC_CLAMP = 0; //Clamp disabled
	VPFE_CCDC_DCSUB = 0;
	VPFE_CCDC_COLPTN = 0xEE44EE44; //Color pattern
	VPFE_CCDC_BLKCMP = 0; //Compensation of black, no activated
	VPFE_CCDC_FPC = 0; //Disable the replace of the pixels defined
					   //on the next line
	VPFE_CCDC_FPC_ADDR = 0; //Replace pixel x with the average of x-1
							//and x+1
	VPFE_CCDC_VDINT = 1; // Ady
	VPFE_CCDC_ALAW = 0; //Apply gamma(A-law) disabled
	VPFE_CCDC_REC656IF = 0x00000003; //Enable de REC656 interface
									 //and FVH error correction

	/*
	* Input format is Cb:Y:Cr:Y, w/ Y in odd-pixel position
	*/
	VPFE_CCDC_CCDCFG = 0x00000800; //Location of Y signal odd pixel
	VPFE_CCDC_FMTCFG = 0;
	VPFE_CCDC_FMT_HORZ = 0x000002D0; //Number of pixels on horizontal
									 //direction=720
	VPFE_CCDC_FMT_VERT = 0x00000240; //Number of pixels on vertical
									 //direction=576
	VPFE_CCDC_FMT_ADDR0 = 0;
	VPFE_CCDC_FMT_ADDR1 = 0;
	VPFE_CCDC_FMT_ADDR2 = 0;
	VPFE_CCDC_FMT_ADDR3 = 0;
	VPFE_CCDC_FMT_ADDR4 = 0;
	VPFE_CCDC_FMT_ADDR5 = 0;
	VPFE_CCDC_FMT_ADDR6 = 0;
	VPFE_CCDC_FMT_ADDR7 = 0;
	VPFE_CCDC_PRGEVEN_0 = 0;
	VPFE_CCDC_PRGEVEN_1 = 0;
	VPFE_CCDC_PRGODD_0 = 0;
	VPFE_CCDC_PRGODD_1 = 0;
	VPFE_CCDC_VP_OUT = 0x041A2D00;
	VPFE_RESZ_PCR=0; //Disabled resizer module
}

/* ------------------------------------------------------------------------ *
* *
* vpbe_init( colorbars/loopback, ntsc/pal, svideo/composite ) *
* *
* ------------------------------------------------------------------------ */

static void vpbe_init( Uint32 colorbar_loopback_mode, Uint32 ntsc_pal_mode,
Uint32 output_mode, Uint32 video_buffer )
{
	//Uint32 video_buffer = mem_buffer_out;
	Uint32 basep_x;
	Uint32 basep_y;
	Uint32 width;
	Uint32 height;

	if ( ntsc_pal_mode == NTSC )
	{
		basep_x = 122;
		basep_y = 18;
		width = 720;
		height = 480;
	}
	else
	{
		basep_x = 132;
		basep_y = 22;
		width = 720;
		height = 576;
	}
	
	/*
	* Setup VPBE
	*/
	VPSS_CLK_CTRL= 0x00000018; //Enable DAC and VENC clock, both at 27 MHz
	VPBE_PCR = 0; // No clock div, clock enable

	/*
	* Setup OSD
	*/
	VPBE_OSD_MODE = 0x000000fc; // Background color blue using clut in
							    // ROM0
	VPBE_OSD_OSDWIN0MD = 0; // Disable both OSD windows and cursor window
	VPBE_OSD_OSDWIN1MD = 0;
	VPBE_OSD_RECTCUR = 0; //Cursor displayed disabled
	VPBE_OSD_VIDWIN0OFST = width>>4 ; // line with in pixels/16
	VPBE_OSD_VIDWIN0ADR = video_buffer; //SDRAM source address Video
										//Window 0
	VPBE_OSD_BASEPX = basep_x; //Sets the maximum image resolution
	VPBE_OSD_BASEPY = basep_y;
	VPBE_OSD_VIDWIN0XP = 0; //Start position
	VPBE_OSD_VIDWIN0YP = 0;
	VPBE_OSD_VIDWIN0XL = width; //Video window width, horizontal
								//display in pixels
	VPBE_OSD_VIDWIN0YL = height >> 1; //Vertical display height
	VPBE_OSD_MISCCTL = 0; //Miscellaneous options
	VPBE_OSD_VIDWINMD = 0x00000003; // Disable vwindow 1 and enable
									// vwindow 0. Frame mode with no
									//up-scaling
	/*
	* Setup VENC
	*/

	if ( ntsc_pal_mode == NTSC )
		VPBE_VENC_VMOD = 0x00000003; // Standard NTSC interlaced output
	else
		VPBE_VENC_VMOD = 0x00000043; // Standard PAL interlaced output
		
//	VPBE_VENC_VIDCTL = 0x030;
	VPBE_VENC_VDPRO = 0x00; //CBMD bit
								//VDPRO es per als colors
	VPBE_VENC_DACTST = 0;
	VPBE_VENC_DACSEL = 0x00004210;

	/*
	* Choose Output mode
	*/

	if ( output_mode == COMPOSITE_OUT )
		VPBE_VENC_DACSEL = 0x00000000;
	else if ( output_mode == SVIDEO_OUT )
		VPBE_VENC_DACSEL = 0x00004210;
}

/* ------------------------------------------------------------------------ *
* *
* video_loopback_test( ) *
* *
* ------------------------------------------------------------------------ */

Int16 video_loopback_test(Uint32 buffer_in, Uint32 buffer_out ){
	Int16 ntsc_pal_mode;
	Int16 output_mode;
	
	//EVMDM6437_DIP_init( );
	
	{
		/* Check Video Settings */
		ntsc_pal_mode = EVMDM6437_DIP_get( JP1_JUMPER ); // NTSC or PAL
		output_mode = EVMDM6437_DIP_get( SW7_SWITCH ); //SVideo/Composite
printf("ntsc_pal_mode:%d\n", ntsc_pal_mode);
		if ( ntsc_pal_mode == NTSC ) {
			if ( output_mode == COMPOSITE_OUT )
				printf( " Video Loopback test: [NTSC][COMPOSITE]\n" );
			else if ( output_mode == SVIDEO_OUT )
				printf( " Video Loopback test: [NTSC][S-VIDEO]\n" );
			else
				return -1;
		}
		else if ( ntsc_pal_mode == PAL )
		{
			if ( output_mode == COMPOSITE_OUT )
				printf( " Video Loopback test: [PAL][COMPOSITE]\n" );
			else if ( output_mode == SVIDEO_OUT )
				printf( " Video Loopback test: [PAL][S-VIDEO]\n" );
			else
				return -1;
		}
		else
			return -2;
		/* Setup Front-End */
		tvp5146_init( ntsc_pal_mode, output_mode );
		vpfe_init( ntsc_pal_mode, buffer_in );

		/* Setup Back-End */
		vpbe_init( LOOPBACK, ntsc_pal_mode, output_mode, buffer_out );/* Wait for the USER to press something useful */
        
    };
    
	return 0;
}
