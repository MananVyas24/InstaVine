
/*
 * vignette.c
 *
 *  Created on: Apr 22, 2014
 *      Author: Shruti
 */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "evmdm6437.h"
#include "evmdm6437_dip.h"
#include "evmdm6437_led.h"
#include "header_file.h"

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
void Copy(void){
	int i, j;

	// Copy data from input buffer to output buffer and
	// draw green square box at the center of view
	for(i=0; i<vHeight; ++i)
		for (j=0; j<vWidth/2; ++j) {
			buffer_out[i * vWidth/2 + j] = buffer_in[i * vWidth/2 + j];
			if ( i >= 220 && i < 260 && j >= 170 && j < 190 )
				buffer_out[i * vWidth/2 + j] = 0;
		}

	// Use block-based processing inside internal memory to speed up
	// Demo code for block seperation: BufferNum blocks along row direction

}
void cacheInit()
{
    volatile Uint32 *marPtr;
    Uint32 i;

    #define SIZE_DDR2         0x10000000
    #define MAR_STEP_SIZE    0x01000000

    CACHE_L1PINV = 1;    // L1P invalidated
    CACHE_L1PCFG = 7;    // L1P on, MAX size
    CACHE_L1DINV = 1;    // L1D invalidated
    CACHE_L1DCFG = 0;    // L1D off
    CACHE_L2INV  = 1;    // L2 invalidated
    CACHE_L2CFG = 3;     // 128k L2 cache enabled
    i = CACHE_L2CFG;

    marPtr = (volatile Uint32 *)0x01848200;    // base of ddr2 @ 0x80000000

    for (i = 0; i < SIZE_DDR2; i+= MAR_STEP_SIZE) {
        *marPtr++ = 1;
    }

    CACHE_L1DCFG = 0x00000004; // grab 32k of cache
    i = CACHE_L1DCFG;    // read register to setup cache mode change
}



int cmpfunc (const void * a, const void * b)
{
   return ( *(int*)a - *(int*)b );
}

void main(void)
{
	//Int16 dip0, dip1, dip2, dip3;
int i,j;
	/* Initialize BSL */
	EVMDM6437_init();
	//cacheInit();
	/* Initialize the DIP Switches & LEDs if needed */
	EVMDM6437_DIP_init();
	EVMDM6437_LED_init();

	video_loopback_test();


	while (1){


		/* Will return DIP_DOWN or DIP_UP
		dip0 = EVMDM6437_DIP_get(DIP_0);
		dip1 = EVMDM6437_DIP_get(DIP_1);
		dip2 = EVMDM6437_DIP_get(DIP_2);
		dip3 = EVMDM6437_DIP_get(DIP_3);*/

		// Run different procedures depending on the DIP switches pressed.
		DirectTransferInput_Proc();

	/*	for(i=0; i<vHeight; i++)
								{
									for (j=0; j<vWidth/2; j++)
									{

											buffer_proc2[i*vWidth/2 +j] = buffer_proc1[i*vWidth/2+j];
										}
								}*/

		vignette_effect();


		DirectTransferProc_Out();

		//printf("frame\n");
		//break;

	}
}

void vignette_effect(void)
{
	int i,j,m=0;
	//ycbcr to rgb
	//extract cb and cr
	ycbcr_to_rgb_quarter();

	//rgb to hsv - basically only hue - not finding s and v because not required
	rgb_to_hsv_quarter();

	//skin color segmentation
	skin_segmentation();

	//find the focal point of the vignette ellipse
	focal_point();

	//fade out the image outside of the vignette radius
	fade();

	//convert rgb back to ycbcr
	rgb_to_ycbcr_quarter();

	//transfer to buffer proc 2 so that it can be moved to output buffer
				for(i=0; i<vHeight; i++)
							{
								for (j=0; j<vWidth/2; j++)
								{
									if(j<HalfWidth/2 && i<HalfHeight)
									{
										buffer_proc2[i*vWidth/2 +j] = buffer_proc1[i*vWidth+2*j];
									}
									else if(j>=HalfWidth/2 && i>=HalfHeight)
									{
										buffer_proc2[i*vWidth/2+j] = buffer_proc_inter[((i-HalfHeight)*vWidth+(2*(j-HalfWidth/2)))];
										if(i-HalfHeight>=332/2 && i-HalfHeight<408/2 && (j-HalfWidth/2)>=274/2 && (j-HalfWidth/2)<=337/2 )
										{

											/*	buffer_proc2[i*vWidth/2 +j] = logo[m*128+n];
												if(n<128-2)
													n=n+2;
												else
												{
													m = m+2;
													n = 0;
												}
												//printf("%d %d\n",m,n);*/
											buffer_proc2[i*vWidth/2+j] = logo_small[m];
											m++;

										}
									}
									else
									{
										buffer_proc2[i*vWidth/2+j] = 0x01800180;
									}

								}

							}


}

void ycbcr_to_rgb_quarter(void)
{
	int i,j,x,a1,b1;
	int y0,cb,cr,y1,r_temp1,g_temp1,b_temp1,r_temp2,g_temp2,b_temp2;

	for (i=0; i<vHeight; i=i+2)
	{
		for(j=0; j<vWidth/2; j++)
		{
			x = i*vWidth/2+j;
			a1 = (i*vWidth)+(2*j+0);
//			b1 = (i*vWidth)+(2*j+1);
		y0 = ((buffer_proc1[x] & 0xFF000000)>>24);
//		y1= (buffer_proc1[x] & 0x0000FF00)>>8;
		cr = (buffer_proc1[x] & 0x00FF0000)>>16;
		cb = (buffer_proc1[x] & 0x000000FF);
		Cb[a1] = cb;
		Cr[a1] = cr;
//		Cb[b1] = cb;
//		Cr[b1] = cr;
		Y[a1] = y0;
//		Y[b1] = y1;

		//buf = buffer_proc1[i];
		//y0 = ((buffer_proc1[i] & 0x000000FF));
		//y1 = (buffer_proc1[i] & 0x00FF0000) >> 16;
		//cr = (buffer_proc1[i] & 0x0000FF00) >> 8;
		//cb = (buffer_proc1[i] & 0xFF000000) >> 24;
		r_temp1 = (10000 * (y0 ) + 13711 * (cr - 128));
//		r_temp2 = (10000 * (y1 ) + 13711 * (cr - 128));
		g_temp1 = (10000 * (y0 ) - 3369 * (cb - 128) - 6892 * (cr - 128));
//		g_temp2 = (10000 * (y1 ) - 3369 * (cb - 128) - 6892 * (cr - 128));
		b_temp1 = (10000 * (y0 ) + 17324 * (cb - 128));
//		b_temp2 = (10000 * (y1 ) + 17324 * (cb - 128));

		r[a1] = r_temp1 < MINI ? MINI : r_temp1 >(MAXP * 10000) ? MAXP : r_temp1 / 10000;
//		r[b1] = r_temp2 < MINI ? MINI : r_temp2 >(MAXP * 10000) ? MAXP : r_temp2 / 10000;
		g[a1] = g_temp1 < MINI ? MINI : g_temp1 >(MAXP * 10000) ? MAXP : g_temp1 / 10000;
//		g[b1] = g_temp2 < MINI ? MINI : g_temp2 >(MAXP * 10000) ? MAXP : g_temp2 / 10000;
		b[a1] = b_temp1 < MINI ? MINI : b_temp1 >(MAXP * 10000) ? MAXP : b_temp1 / 10000;
//		b[b1] = b_temp2 < MINI ? MINI : b_temp2 >(MAXP * 10000) ? MAXP : b_temp2 / 10000;
		}
	}
}

void rgb_to_hsv_quarter()
{
	//we need only hue
	int i,j;
	int min;    //Min. value of RGB
	int max;    //Max. value of RGB
	int delMax; //Delta RGB value
	unsigned char red,green,blue;


	for (i=0;i<vHeight;i=i+2)
	{
	     for (j=0;j<vWidth;j=j+2)
	     {
	    	 red = r[i*vWidth+j];
	    	 green = g[i*vWidth+j];
	    	 blue = b[i*vWidth+j];

			if (red > green) { min = green; max = red; }
			else { min = red; max = green; }
			if (blue > max) max = blue;
			if (blue < min) min = blue;

			delMax = max - min;

			if ( delMax == 0 ) { hue[i*vWidth+j] = 0; }
			else
			{

				if ( red == max )
					hue[i*vWidth+j] = (abs((green - blue)*60)/delMax);
				else if ( green == max )
					hue[i*vWidth+j] = ( 120 +  abs((blue - red)*60)/delMax);
				else if ( blue == max )
					hue[i*vWidth+j] = ( 240 +  abs((red - green)*60)/delMax);
			}
	     }
	}
}

void skin_segmentation(void)
{
	int i,j,min,max;
	unsigned char red,green,blue;

	for (i=0;i<vHeight;i=i+2)
		{
			for(j=0;j<vWidth;j=j+2)
			{
				//if  ((int)Cr[i*vWidth+j]>=140 && (int)Cr[i*vWidth+j]<=165 && (int)Cb[i*vWidth+j]>=140 && (int)Cb[i*vWidth+j]<=195 && hue[i*vWidth+j]>=3.6/*0.01*/ && hue[i*vWidth+j]<=36/*0.1*/)
				if  ((hue[i*vWidth+j]<0.1*360)||(hue[i*vWidth+j]>0.9*360))
								{
									//printf("found");
									if(Cr[i*vWidth+j]<= (1.5*(float)Cb[i*vWidth+j]+20.0) && Cr[i*vWidth+j]>= (0.34*(float)Cb[i*vWidth+j]+76.21) && Cr[i*vWidth+j]>=(-4.5652*(float)Cb[i*vWidth+j]+234.56) && Cr[i*vWidth+j]<= (-1.15*(float)Cb[i*vWidth+j]+301.75) && Cr[i*vWidth+j]<= (-2.29*(float)Cb[i*vWidth+j]+432.85) )
									{
										//find max{R,G,B} and min{R,G,B}
										red = r[i*vWidth+j];
										green = g[i*vWidth+j];
										blue = b[i*vWidth+j];
										if (red > green) { min = (int)green; max = (int)red; }
										else { min = (int)red; max = (int)green; }
										if (blue > max) max = (int)blue;
										if (blue < min) min = (int)blue;
										if(
											((int)red>95 && (int)green>40 && (max-min)>15 && (int)blue>20 && (int)abs(red-green)>15 && ((int)red>(int)green) && ((int)red>(int)blue))
											||
											((int)red>220 && (int)green>210 && (int)blue>170 && (int)abs(red-green)<=15 && (red>blue) && (green>blue))
										  )
										{
											//if(Y[i*vWidth+j]<200)
											//cout<<"found"<<endl;
											segment[i*vWidth+j]=255;
										}
									}
								}
				else
					segment[i*vWidth+j]=0;
			}
		}
}

void focal_point(void)
{
/*	int leftmost=vWidth;
	int rightmost=0;
	int bottommost=0;
	int topmost=vHeight;*/
	int i,j,max_density=0,max_dens_index=0;


	//calculate column-wise density of white pixels
	for(j=0;j<vWidth;j=j+2)
	{
		density[j]=0;
		for(i=0;i<vHeight;i=i+2)
		{
			if((int)segment[i*vWidth+j]!=0)
			{
				density[j] = density[j]+1;
			}

		}
		//if(density[j]!=0)
			//printf("%d: %d\n",j,density[j]);
	}

	//calculate the max density value
	for(j=0;j<vWidth;j=j+2)
	{
		if(max_density<density[j])
		{
			max_density = density[j];
		}

	}
	//find the index for max density
	for(j=0;j<vWidth;j=j+2)
	{
		if(max_density==density[j])
		{
			max_dens_index = j;
			//printf("max density %d, index %d\n",max_density,j);
			break;
		}
	}
 j = max_dens_index;

	for (i=0;i<vHeight;i=i+2)
	{
	    //for (j=0;j<vWidth;j++)
	    {
	        if((int)segment[i+vWidth+j]!=0 )//&& density[j]> max_density/5)
	        {
	            if(i<topmost)
	                topmost = i;

	            if(i>bottommost)
	                bottommost = i;

	//            if(j<leftmost)
	    //            leftmost = j;

	  //          if(j>rightmost)
	      //          rightmost = j;

	        }
	    }
	}
	center_row = abs(topmost-bottommost)/2;
	if(center_row%2 !=0)
	{
		center_row = center_row+1;
	}
	//center_col = (leftmost+rightmost)/2;
	center_col = max_dens_index;
	//printf("%d %d %d %d",topmost,bottommost,center_row,center_col);
}

void fade(void)
{
	int radius,grade,i,j;
	int center_r,center_c;
	double L,f,Ln;

	//Parameters: change these to change the size of the vignette
	radius = 2;
	grade = 2;

	center_r = (center_row>(vHeight-center_row))?center_row:(vHeight-center_row);
	center_c = (center_col>(vWidth-center_col))?center_col:(vWidth-center_col);
	L = sqrt(pow(center_r,2)+pow(center_c,2));

	for (i = 0;i<vHeight;i=i+2)
	{
	  for (j = 0;j<vWidth;j=j+2)
	  {
	    Ln = sqrt(pow(center_row-i,2) + pow(center_col-j,2));

	    f = (Ln-L)/L;

	    f = ((double)radius*pow(f,(double)grade));
	    if(f>1)
	    {
	      f=1;
	    }
	    if(i==center_row ||i+2 ==center_row ||i-2==center_row ||j==center_col||j-2==center_col||j+2==center_col)
	    {
	    	r[i*vWidth+j] = (unsigned char)255;
	    	g[i*vWidth+j] = (unsigned char)0;
	    	b[i*vWidth+j] = (unsigned char)0;
	    }
	    else
	    {
	    	r[i*vWidth+j] = (unsigned char)(f*(int)r[i*vWidth+j]);
	    	g[i*vWidth+j] = (unsigned char)(f*(int)g[i*vWidth+j]);
	    	b[i*vWidth+j] = (unsigned char)(f*(int)b[i*vWidth+j]);
	    }
	  }
	}
}

void rgb_to_ycbcr_quarter(void)
{
	int i,j,x,a1,b1;
	int y0,cb,cr,y1,r_temp1,g_temp1,b_temp1,r_temp2,g_temp2,b_temp2;
	int cb1,cr1,y2,cb2,cr2;

	for (i=0;i<vHeight;i=i+2)
	{
		for(j=0; j<vWidth/2;j++)
		{
			x = i*vWidth/2+j;
			a1 = (i*vWidth)+(2*j+0);
//			b1 = (i*vWidth)+(2*j+1);
						//buf = buffer_proc1[i];
						r_temp1 = r[a1];
						g_temp1 = g[a1];
						b_temp1 = b[a1];
//						r_temp2 = r[b1];
//						g_temp2 = g[b1];
//						b_temp2 = b[b1];
						y1 = ((2990*r_temp1) + (5870*g_temp1) + (1140*b_temp1));
						cb1 = ((-1689*r_temp1) - (3317*g_temp1) + (5006*b_temp1));
						cr1 = (4998*r_temp1) - (4185*g_temp1) - (812*b_temp1);
//						y2 = ((2990*r_temp2) + (5870*g_temp2) + (1140*b_temp2));
						/*y1 = ((2990*segment[a1]) + (5870*segment[a1]) + (1140*segment[a1]));
						cb1 = ((-1689*segment[a1]) - (3317*segment[a1]) + (5006*segment[a1]));
						cr1 = (4998*segment[a1]) - (4185*segment[a1]) - (812*segment[a1]);
						y2 = ((2990*segment[b1]) + (5870*segment[b1]) + (1140*segment[b1]));*/
						//cb2 = ((-1689*r[2*i+1]) - (3317*g[2*i+1]) + (5006*b[2*i+1]));
						//cr2 = (4998*r[2*i+1]) - (4185*g[2*i+1]) - (812*b[2*i+1]);
						y1 = (int)((y1/10000)*219/256);
						cb1 = (int)((cb1/10000)*224/256) +128;
						cr1 = (int)((cr1/10000)*224/256) +128;
//						y2 = (int)((y2/10000)*219/256);
						//cb2 = (int)((cb2/10000)*224/256) +128;
						//cr2 = (int)((cr2/10000)*224/256) +128;
						//cb = (cb1+cb2)/2;
						//cr = (cr1+cr2)/2;
						if(y1<0)
							y1 = 1;
						else if(y1>255)
							y1 = 255;
//						if(y2<0)
//							y2 = 1;
//						else if(y2>255)
//							y2 = 255;
						if(cb1<0)
							cb1 = 0;
						else if(cb1>255)
							cb1 = 255;
						if(cr1<0)
							cr1 = 0;
						else if(cr1>255)
							cr1 = 255;


						buffer_proc_inter[x] = ((y1 & 0x000000FF)<<24)|((cr1 & 0x000000FF)<<16)|((y1 & 0x000000FF)<<8)|(cb1 & 0x000000FF);

					}
	}
}



