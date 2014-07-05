/*
 *  ======== main.c ========
 */

#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Timestamp.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Swi.h>
//#include <ti/sysbios/hal/Hwi>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <xdc/cfg/global.h>

#include <stdio.h>
#include <stdlib.h>
#include "evmdm6437.h"
#include "evmdm6437_dip.h"
#include "evmdm6437_led.h"
#include "fonts.h"
#include "text_functions.h"

extern Int16 video_loopback_test();

// size for buffer_in: 720 * 480 / 2, the reason is explained below. 
#define Pixels 172800

// Resolution 720 * 480 (NTSC mode)
#define vWidth 720
#define vHeight 480

// CAN change the internal blocksize here, the example is 60 * 120
#define INTERNAL_BLK_WIDTH 60
#define INTERNAL_BLK_HEIGHT 120

struct display disp;
char str_gesture[200];	//variable used for printing text to screen

extern struct abc_font_struct tahomabold_20[];

	
//////////////////////////////////////////////////////////////////////
Uint32 buffer_out[Pixels]; 
Uint32 buffer_in[Pixels]; 
Uint32 buffer_proc_in[Pixels];
Uint32 buffer_proc_out[Pixels];

// buffer_in represents one input frame which consists of two interleaved frames.
// Each 32 bit data has the information for two adjacent pixels in a row.
// Thus, the buffer holds 720/2 integer data points for each row of 2D image and there exist 480 rows.
//
// Format: yCbCr422 ( y1 | Cr | y0 | Cb )
// Each of y1, Cr, y0, Cb has 8 bits
// For each pixel in the frame, it has y, Cb, Cr components
//
// You can generate a lookup table for color conversion if you want to convert to different color space such as RGB.
// Could refer to http://www.fourcc.org/fccyvrgb.php for conversion between yCbCr and RGB
// 

// Direct copy data from input buffer to output buffer; both on the external memories
// We do NOT use EDMA here
void Copy(void){
	int i,j;
	unsigned char tmp = 128;
	int k;
		
	memcpy(buffer_proc_in, buffer_in, sizeof(unsigned int)*Pixels);
	
	for(i=0; i<vHeight; ++i) {
		for (j=0; j<vWidth/2; ++j) {
			if ( i >= 220 && i < 260 && j >= 170 && j < 190 )
				buffer_proc_in[i * vWidth/2 + j] = 0;			
		}
    }
    
	disp.buffer = buffer_proc_in;

	print_support_text(disp);		
	sprintf(str_gesture, "Only white letters are supported\n\0"); 		
	print_string(str_gesture,20,200,disp);
	sprintf(str_gesture, "The String SHOULD end with null terminator\nRefer to the example in the code\n\0");
	print_string(str_gesture,20,250,disp);
	sprintf(str_gesture, "BUG:: With EDMA, msg can be broken\n\0");
	print_string(str_gesture,20,350,disp);
	
	memcpy(buffer_out, buffer_proc_in, sizeof(unsigned int)*Pixels);
}

//Main program
void myIdelFunc()
{
	Copy();
}

/*
 *  ======== main ========
 */
Void main()
{ 
	/* Initialize BSL */
	EVMDM6437_init();
	
    /* Initialize the DIP Switches & LEDs if needed */
    EVMDM6437_DIP_init( );
    EVMDM6437_LED_init( );
    
    // Setup screen log environment
    disp.width = vWidth;
	disp.height = vHeight;
	
	video_loopback_test(buffer_in, buffer_out);
	
    BIOS_start();     /* enable interrupts and start SYS/BIOS */
}
