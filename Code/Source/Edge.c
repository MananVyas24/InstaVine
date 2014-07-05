#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "evmdm6437.h"
#include "evmdm6437_dip.h"
#include "evmdm6437_led.h"
//#include <IMG_ycbcr422pl_to_rgb565.h>
// ---- *************** OVERFLOW ERRORS FIXED ******************
/////////////////////////////////////////////////////////////////
//         DON'T change the following setting for EDMA         //
/////////////////////////////////////////////////////////////////
/* EDMA Registers for 6437*/
#define		PaRAM_OPT	0	// Channel Options Parameter
#define		PaRAM_SRC	1	// Channel Source Address
#define		PaRAM_BCNT	2	// Count for 2nd Dimension (BCNT) | Count for 1st Dimension (ACNT)
#define		PaRAM_DST	3	// Channel Destination Address
#define		PaRAM_BIDX	4	// Destination BCNT Index | Source BCNT Index
#define		PaRAM_RDL	5	// BCNT Reload (BCNTRLD) | Link Address (LINK)
#define		PaRAM_CIDX	6	// Destination CCNT Index | Source CCNT Index
#define		PaRAM_CCNT	7	// Count for 3rd Dimension (CCNT)

/* EDMA Registers for 6437*/
#define		EDMA_IPR	*(volatile int *)0x01C01068	// EDMA Channel interrupt pending low register
#define		EDMA_IPRH	*(volatile int *)0x01C0106C	// EDMA Channel interrupt pending high register
#define		EDMA_IER	*(volatile int *)0x01C01050	// EDMA Channel interrupt enable low register
#define		EDMA_IERH	*(volatile int *)0x01C01054	// EDMA Channel interrupt enable high register
#define		EDMA_ER 	*(volatile int *)0x01C01000	// EDMA Event low register
#define		EDMA_ERH	*(volatile int *)0x01C01004	// EDMA Event high register
#define		EDMA_EER	*(volatile int *)0x01C01020	// EDMA Event enable low register
#define		EDMA_EERH	*(volatile int *)0x01C01024	// EDMA Event enable high register
#define		EDMA_ECR	*(volatile int *)0x01C01008	// EDMA Event clear low register
#define		EDMA_ECRH	*(volatile int *)0x01C0100C	// EDMA Event clear high register
#define		EDMA_ESR	*(volatile int *)0x01C01010	// EDMA Event set low register
#define		EDMA_ESRH	*(volatile int *)0x01C01014	// EDMA Event set high register

/////////////////////////////////////////////////////////////////

extern Int16 video_loopback_test();

// size for buffer_in: 720 * 480 / 2, the reason is explained below. 
#define Pixels 172800

// Resolution 720 * 480 (NTSC mode)
#define vWidth 720
#define vHeight 480

// CAN change the internal blocksize here, the example is 60 * 120
#define INTERNAL_BLK_WIDTH 60
#define INTERNAL_BLK_HEIGHT 120

#define MAXP 240
#define MINI 16

// Define a space on memory for save the information input and output (Interface data)
Uint32 buffer_out[Pixels]; //from 0x80000000
Uint32 buffer_in[Pixels]; //from 0x800A8C00, which is the same as 4 (bytes for integer) * Pixels
Uint32 buffer_proc1[Pixels]; //from 0x80151800
Uint32 buffer_proc2[Pixels];
Uint32 buffer_proc_inter[Pixels];
unsigned char r[Pixels*2];
unsigned char g[Pixels*2];
unsigned char b[Pixels*2];
unsigned char OutputImageR[vWidth*vHeight];
unsigned char OutputImageG[vWidth*vHeight];
unsigned char OutputImageB[vWidth*vHeight];
int factor = 1; // FACTOR = 3 ; BIAS = 1 GIVES CARTOONISH EFFECT
int bias = 0;
int Mask[3][3] = {
	                   	{1,  1, 1},
                      	{1, -7, 1},
                       	{1,  1, 1}
               	   };
double red = 0.0;
double green = 0.0;
double blue = 0.0;
int ii;	int jj; int h; int l;
int R; int G; int B;

// Declare the internal buffer 
Uint32 internal_buffer_2D[INTERNAL_BLK_HEIGHT][INTERNAL_BLK_WIDTH / 2];



// Define the position of the data (refer to linker.cmd)
// Internal memory L2RAM ".l2ram" 
// External memory DDR2 ".ddr2"
#pragma DATA_SECTION(buffer_out,".ddr2")
#pragma DATA_SECTION(buffer_in,".ddr2")
#pragma DATA_SECTION(buffer_proc1,".ddr2")
#pragma DATA_SECTION(buffer_proc2,".ddr2")
#pragma DATA_SECTION(buffer_proc_inter,".ddr2")
#pragma DATA_SECTION(r,".ddr2")
#pragma DATA_SECTION(g,".ddr2")
#pragma DATA_SECTION(b,".ddr2")
#pragma DATA_SECTION(OutputImageR,".ddr2");
#pragma DATA_SECTION(OutputImageG,".ddr2");
#pragma DATA_SECTION(OutputImageB,".ddr2");

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


// Using EDMA, copy data from input buffer in the external memory to internal buffer in the internal memory.


// Using EDMA, copy data from input buffer to output buffer; both on the external memory
void DirectTransferInput_Proc(void){
	int i;
	int *Event;

	// DON'T change the following setting except the source and destination address
	// Event[PaRAM_SRC] is the source data
	// Event[PaRAM_DST] is the destination data
	// Direct copy from External to External
	Event = (int *)(0x01C04000 + 32 * 9);
	Event[PaRAM_OPT] = 0x0010000C;
	Event[PaRAM_SRC] = (int)buffer_in;			// Source address
	Event[PaRAM_BCNT] = ((vHeight) << 16) | (vWidth / 2 * 4);
	Event[PaRAM_DST] = (int)buffer_proc1;  	// Destination address
	Event[PaRAM_BIDX] = ((vWidth / 2 * 4) << 16) | (vWidth / 2 * 4);
	Event[PaRAM_RDL] = 0x0000FFFF;  //
	Event[PaRAM_CIDX] = 0x00000000;
	Event[PaRAM_CCNT] = 0x00000001;

	// DON'T change the following setting
	for (i = 0; i < 500; i++)
		if (EDMA_IPR & 0x400 == 0) break; // Waiting for EDMA channel 9 transfer complete	
	EDMA_IPR = 0x200;             // Clear CIP9
	EDMA_ESR = EDMA_ESR | 0x200;    // Start channel 9 EDMA transfer
	
	for( i = 0; i < 500; i++){
		if(EDMA_IPR & 0x200 ==0) break;
		//printf("%d\n",i);
	}
}

void DirectTransferProc_Out(void){
	int i;
	int *Event;

	// DON'T change the following setting except the source and destination address
	// Event[PaRAM_SRC] is the source data
	// Event[PaRAM_DST] is the destination data
	// Direct copy from External to External
	Event = (int *)(0x01C04000 + 32 * 10);
	Event[PaRAM_OPT] = 0x0010000C;
	Event[PaRAM_SRC] = (int)buffer_proc2;			// Source address
	Event[PaRAM_BCNT] = ((vHeight) << 16) | (vWidth / 2 * 4);
	Event[PaRAM_DST] = (int)buffer_out;  	// Destination address
	Event[PaRAM_BIDX] = ((vWidth / 2 * 4) << 16) | (vWidth / 2 * 4);
	Event[PaRAM_RDL] = 0x0000FFFF;  //
	Event[PaRAM_CIDX] = 0x00000000;
	Event[PaRAM_CCNT] = 0x00000001;

	// DON'T change the following setting
	for (i = 0; i < 500; i++)
	if (EDMA_IPR & 0x200 == 0) break; // Waiting for EDMA channel 10 transfer complete	
	EDMA_IPR = 0x400;             // Clear CIP10
	EDMA_ESR = EDMA_ESR | 0x400;    // Start channel 10 EDMA transfer
	
	for( i = 0; i < 500; i++){
		if(EDMA_IPR & 0x400 ==0) break;
		//printf("%d\n",i);
	}
}

void main(void)
{
	//Int16 dip0, dip1, dip2, dip3;
	Uint32 i,j,k;
	//int *inp= (int*)buffer_out;
/*	unsigned char yc[Pixels*4];
	unsigned char y[Pixels*2];
	unsigned char cr[Pixels];
	unsigned char cb[Pixels];
	unsigned char rgb[Pixels*2];*/
//	unsigned char r[Pixels*2];
//	unsigned char g[Pixels*2];
//	unsigned char b[Pixels*2];
	int y0,cb,cr,y1,r_temp1,g_temp1,b_temp1,r_temp2,g_temp2,b_temp2;
	int cb1,cr1,y2,cb2,cr2;
	Uint32 buf;
	//short coeff[] = { 0x2000, 0x2BDD, -0x0AC5, -0x1658, 0x3770 };
	//unsigned char *temp;

	/* Initialize BSL */
	EVMDM6437_init();
	/* Initialize the DIP Switches & LEDs if needed */
	EVMDM6437_DIP_init();
	EVMDM6437_LED_init();

	video_loopback_test();

	while (1){

		DirectTransferInput_Proc();

		for (i=0; i<Pixels; i++)
		{
			y0 = ((buffer_proc1[i] & 0xFF000000)>>24);
			y1= (buffer_proc1[i] & 0x0000FF00)>>8;
			cr = (buffer_proc1[i] & 0x00FF0000)>>16;
			cb = (buffer_proc1[i] & 0x000000FF);
			buf = buffer_proc1[i];
			//y0 = ((buffer_proc1[i] & 0x000000FF));
			//y1 = (buffer_proc1[i] & 0x00FF0000) >> 16;
			//cr = (buffer_proc1[i] & 0x0000FF00) >> 8;
			//cb = (buffer_proc1[i] & 0xFF000000) >> 24;
			r_temp1 = (10000 * (y0 ) + 13711 * (cr - 128));
			r_temp2 = (10000 * (y1 ) + 13711 * (cr - 128));
			g_temp1 = (10000 * (y0 ) - 3369 * (cb - 128) - 6892 * (cr - 128));
			g_temp2 = (10000 * (y1 ) - 3369 * (cb - 128) - 6892 * (cr - 128));
			b_temp1 = (10000 * (y0 ) + 17324 * (cb - 128));
			b_temp2 = (10000 * (y1 ) + 17324 * (cb - 128));
			
			r[2 * i + 0] = r_temp1 < MINI ? MINI : r_temp1 >(MAXP * 10000) ? MAXP : r_temp1 / 10000;
			r[2 * i + 1] = r_temp2 < MINI ? MINI : r_temp2 >(MAXP * 10000) ? MAXP : r_temp2 / 10000;
			g[2 * i + 0] = g_temp1 < MINI ? MINI : g_temp1 >(MAXP * 10000) ? MAXP : g_temp1 / 10000;
			g[2 * i + 1] = g_temp2 < MINI ? MINI : g_temp2 >(MAXP * 10000) ? MAXP : g_temp2 / 10000;
			b[2 * i + 0] = b_temp1 < MINI ? MINI : b_temp1 >(MAXP * 10000) ? MAXP : b_temp1 / 10000;
			b[2 * i + 1] = b_temp2 < MINI ? MINI : b_temp2 >(MAXP * 10000) ? MAXP : b_temp2 / 10000;

		}
		
		// -------------------------- SHARPENING FILTER ---------------------------------
     	
    	for (ii=1; ii<vHeight-1; ii++)
    	{
        	for (jj=1; jj<vWidth-1; jj++)
        	{
            	red = 0.0;
            	green = 0.0;
            	blue = 0.0;

           		for (h=-1; h<=1; h++)
            	{
                	for (l=-1; l<=1; l++)
                	{
                    	//int imageX = (i-3/2+h+vHeight) % vHeight; 
                    	//printf ("LocnX : %d\n",((ii+h)*vWidth)+(jj+k));
                    	//int imageY = (j-3/2+k+vWidth) % vWidth; 
                    	//printf ("  LocnY : %d\n",imageY);
                    	red += r[((ii+h)*vWidth)+(jj+l)] * Mask[h+1][l+1];
                    	green += g[((ii+h)*vWidth)+(jj+l)] * Mask[h+1][l+1];
                    	blue += b[((ii+h)*vWidth)+(jj+l)] * Mask[h+1][l+1];
                	}
            	}

            	R = (factor)*red + (bias);
            	G = (factor)*green + (bias);
            	B = (factor)*blue + (bias);

            	if (R<0) {R=16;}
            	else if (R>255) {R=240;}

            	if (G<0) {G=16;}
            	else if (G>255) {G=240;}

            	if (B<0) {B=16;}
            	else if (B>255) {B=240;}

            	OutputImageR[(ii*vWidth)+(jj)] = (unsigned char)(R);
            	OutputImageG[(ii*vWidth)+(jj)] = (unsigned char)(G);
            	OutputImageB[(ii*vWidth)+(jj)] = (unsigned char)(B);
        	}
    	}

      	
    
    	// Set Output
    	for(ii=0;ii<vHeight;ii++)
    		for (jj=0;jj<vWidth;jj++)
    		{	
    			r[(ii*vWidth)+(jj)] = OutputImageR[(ii*vWidth)+(jj)];// FOR SIMPLE EDGE DETECTION :(255) - OutputImageR[(ii*vWidth)+(jj)];
    			g[(ii*vWidth)+(jj)] = OutputImageG[(ii*vWidth)+(jj)];//(255) - OutputImageG[(ii*vWidth)+(jj)];
    			b[(ii*vWidth)+(jj)] = OutputImageB[(ii*vWidth)+(jj)];//(255) - OutputImageB[(ii*vWidth)+(jj)];
    			//printf("R: %d \n",r[(ii*vWidth*BytesPerPixel)+(jj*BytesPerPixel)]);
    			//printf("G: %d \n",g[(ii*vWidth*BytesPerPixel)+(jj*BytesPerPixel)]);
    			//printf("B: %d \n",b[(ii*vWidth*BytesPerPixel)+(jj*BytesPerPixel)]);
    		}
        
        // --------------------------------  
		
		
		for (i=0;i<Pixels;i++)
				{
					buf = buffer_proc1[i];
					r_temp1 = r[2*i+0];
					g_temp1 = g[2*i+0];
					b_temp1 = b[2*i+0];
					y1 = ((2990*r[2*i+0]) + (5870*g[2*i+0]) + (1140*b[2*i+0]));
					cb1 = ((-1689*r[2*i+0]) - (3317*g[2*i+0]) + (5006*b[2*i+0]));
					cr1 = (4998*r[2*i+0]) - (4185*g[2*i+0]) - (812*b[2*i+0]);
					y2 = ((2990*r[2*i+1]) + (5870*g[2*i+1]) + (1140*b[2*i+1]));
					//cb2 = ((-1689*r[2*i+1]) - (3317*g[2*i+1]) + (5006*b[2*i+1]));
					//cr2 = (4998*r[2*i+1]) - (4185*g[2*i+1]) - (812*b[2*i+1]);
					y1 = (int)((y1/10000)*219/256);
					cb1 = (int)((cb1/10000)*224/256) +128;
					cr1 = (int)((cr1/10000)*224/256) +128;
					y2 = (int)((y2/10000)*219/256);
					//cb2 = (int)((cb2/10000)*224/256) +128;
					//cr2 = (int)((cr2/10000)*224/256) +128;
					//cb = (cb1+cb2)/2;
					//cr = (cr1+cr2)/2;
					if(y1<0)
						y1 = 1;
					else if(y1>255)
						y1 = 255;
					if(y2<0)
						y2 = 1;
					else if(y2>255)
						y2 = 255;
					if(cb1<0)
						cb1 = 0;
					else if(cb1>255)
						cb1 = 255;
					if(cr1<0)
						cr1 = 0;
					else if(cr1>255)
						cr1 = 255;


					buffer_proc_inter[i] = ((y1 & 0x000000FF)<<24)|((cr1 & 0x000000FF)<<16)|((y2 & 0x000000FF)<<8)|(cb1 & 0x000000FF);
					//buffer_proc_inter[i] = ((y2 & 0x000000FF) << 16) | ((cr & 0x000000FF) << 8) | ((y1 & 0x000000FF)) | ((cb & 0x000000FF)<<24);
				//	if ( i >= 10000 && i < 100000 )
				//	{
				//		buffer_proc_inter[i] = 0;	//green box
				//		//printf("%d, %d\n",i,j);
				//	}
				}
			for(i=0; i<vHeight; i++)
			{
				for (j=0; j<vWidth/2; j++)
				{

					buffer_proc2[i*vWidth/2+j] = buffer_proc_inter[i*vWidth/2+j];

				}
			}

		DirectTransferProc_Out();
	}
}