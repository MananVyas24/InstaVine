/*
 * edge_detection.c
 *
 *  Created on: Apr 3, 2014
 *      Author: Shruti
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "evmdm6437.h"
#include "evmdm6437_dip.h"
#include "evmdm6437_led.h"
#include "header_file.h"
//#include "logo.h"

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

	/* Initialize BSL */
	EVMDM6437_init();
	//cacheInit();
	/* Initialize the DIP Switches & LEDs if needed */
	EVMDM6437_DIP_init();
	EVMDM6437_LED_init();

	video_loopback_test();
	//gaussian filter the gray scale image
						//	gaussian(2);

	while (1){


		/* Will return DIP_DOWN or DIP_UP
		dip0 = EVMDM6437_DIP_get(DIP_0);
		dip1 = EVMDM6437_DIP_get(DIP_1);
		dip2 = EVMDM6437_DIP_get(DIP_2);
		dip3 = EVMDM6437_DIP_get(DIP_3);*/

		// Run different procedures depending on the DIP switches pressed.
		DirectTransferInput_Proc();

		pencil_sketch();

		DirectTransferProc_Out();

		//printf("frame\n");


	}
}

void pencil_sketch(void)
{
	//int buf,buf1;
	//Uint32 buf2,buf3;
	int i,j,m;//,ext=WINDOW_SIZE/2;
	//int diff[256];
	//int count_hist[256];
	//int pixel_val[256];
	int amt_edgeinfo,perimeter,Th_max;
	//unsigned char window[WINDOW_SIZE][WINDOW_SIZE];
	//int *inp= (int*)buffer_out;

	//convert image to gray scale by extracting only the y components
			grayscale();
	//gaussian_filter();
			gradient();
			//find possible edges and dif_total
			    calc_poss_edges();

			    //find the histogram

			    for(i=0;i<255;i++)
			    {
			    	pixel_val[i] = i;
			    }
			    histogram();

			    //find differential value

			    diff[0] = count_hist[0];
			    for(i=1;i<256;i++)
			    	diff[i] = count_hist[i] - count_hist[i-1];

			    //gradient histogram difference diagram
			    Th_max = 255;
			    for(i=255;i>=0;i--)
			    {
			    	if(diff[i] == 0)
			    		Th_max = i-1;
			    }

			    //classifying the image based on amount of edge information
			    amt_edgeinfo = 0;
			    for(i=0;i<vHeight;i++)
			    {
			    	for(j=HalfWidth;j<vWidth;j++)
			    	{
			    		if((int)gradient_mag[i*vWidth+j]!=0)
			    			amt_edgeinfo++;

			    	}
			    }
			    perimeter = 2*(vHeight+HalfWidth);

			    //calculating high and low thresholds
			    dif_total = 0;
			    	calc_thresholds(amt_edgeinfo,perimeter,Th_max);




			    edge_mapping();

			    sketch();

	/*			for(i=0;i<vHeight;i++)
				{
					for(j=0;j<vWidth;j++)
					{
						poss_edge[i*vWidth+j] *=255;
					}
				}*/
				//cout<<Th_h<<" "<<Th_l<<endl;


			    m=0;
		    //display the sketch
			for(i = 0; i<vHeight; i++)
			{
				for(j = 0; j<vWidth/2; j++)
				{
					//buf = (int)buffer_gray[i*vWidth/2+(2*j+1)];
					//buf1 = (int)buffer_gray[i*vWidth/2+(2*j+0)];

					if(j<HalfWidth/2)
					{
						buffer_proc2[i*vWidth/2 +j] = buffer_proc1[i*vWidth/2+j];
					}
					else
					{
						if(i>=332 && i<408 && j>=274 && j<=337 )
						{
							buffer_proc2[i*vWidth/2 +j] = logo[m];
							m++;
						}
						else
						{
							buffer_proc2[i*vWidth/2 +j] = (Uint32) ((((int)edge_map[i*vWidth+(2*j+1)]) & 0x000000FF)<<24)|((((int)edge_map[i*vWidth+(2*j+0)]) & 0x000000FF)<<8)| 0x00800080;
						}

					}
					//buf2 = buffer_proc1[i*vWidth/2+j];
					//buf3 = buffer_proc2[i*vWidth/2+j];

				}
			}
			//printf("%d",m);
}

void grayscale(void)
{
	Uint32 i,j,buf1,buf2;
	int buf_temp1,buf_temp2;
	for(i = 0; i < vHeight; i++)
	{

		for(j = HalfWidth/2; j < vWidth/2; j++)
		{

			//buffer_proc2[i*vWidth/2+j] = (buffer_proc1[i*vWidth/2+j] & 0xFF00FF00)| 0x00800080;
			//buf2 = buffer_proc2[i*vWidth/2];

			buffer_gray[i*vWidth+(2*j+1)] = (unsigned char)((buffer_proc1[i*vWidth/2+j] & 0xFF000000)>>24);
			//buf_temp1 = (int)buffer_gray[i*vWidth+(2*j+1)];
			buffer_gray[i*vWidth+(2*j+0)] = (unsigned char)((buffer_proc1[i*vWidth/2+j] & 0x0000FF00)>>8);
			//buf_temp2 = (int)buffer_gray[i*vWidth+(2*j+0)];
		}
	}
}


void  gaussian(int sigma)
 {
     int i,j;
     int ext = WINDOW_SIZE/2;
     int val = -ext;
     double sum = 0;
     int G_x[WINDOW_SIZE][WINDOW_SIZE], G_y[WINDOW_SIZE][WINDOW_SIZE];
     double Gauss[WINDOW_SIZE][WINDOW_SIZE];


     for(i =0;i<WINDOW_SIZE;i++)
     {
         for(j = 0;j<WINDOW_SIZE;j++)
         {
            G_x[i][j]=val;
            G_y[j][i]=val;
         }
         val++;
     }
     for(i=0;i<WINDOW_SIZE;i++)
     {
         for(j=0;j<WINDOW_SIZE;j++)
         {
            double ex = (pow(G_x[i][j],2)/(2*(pow(sigma,2)))) + (pow(G_y[i][j],2)/(2*(pow(sigma,2))));
            Gauss[i][j] = exp(-ex);
            sum += Gauss[i][j];
         }
     }
     //cout<<sum<<endl;
     ////normalization
     for(i=0;i<WINDOW_SIZE;i++)
     {
         for(j=0;j<WINDOW_SIZE;j++)
         {
            Gauss_norm[i][j] = Gauss[i][j]/sum;
            //cout<<Gauss_norm[i][j]<<" ";
         }
         //cout<<endl;
     }
 }

void gaussian_filter()
{
    int i,j,m,n,avg;
    double temp;
    int ext = WINDOW_SIZE/2;
    unsigned char window[WINDOW_SIZE][WINDOW_SIZE];
    double mul[WINDOW_SIZE][WINDOW_SIZE];
    double mul_sum = 0;
    double sum =0.0;

    for(i = ext;i< (vHeight-ext);i++)
    {
          for(j = HalfWidth+ext; j< (vWidth-ext);j++)
          {
            //form a window
            for(m=0;m<WINDOW_SIZE;m++)
            {
                for(n=0;n<WINDOW_SIZE;n++)
                {
	                 window[m][n] = buffer_gray[(i-ext+m)*vWidth+(j-ext+n)];
                }
            }
            temp = 0.0;
            //multiply the window with Gaussian:
            //cout<<"mul"<<endl;

            for(m =0; m<WINDOW_SIZE;m++)
            {
                for(n = 0; n<WINDOW_SIZE; n++)
                {
                    temp = Gauss_norm[m][n];
                    mul[m][n] = window[m][n]*temp;
                    sum+= Gauss_norm[m][n];
                    //cout<<mul[m][n]<<" ";
                }
               // cout<<endl;
            }
            //mul[m/2][n/2] = 0;
            //find the average value with which to replace :
            mul_sum = 0;
            for(m=0; m<WINDOW_SIZE; m++)
            {
                for(n = 0; n<WINDOW_SIZE; n++)
                {
                        mul_sum += mul[m][n];
                }
            }
            //cout<<"mul_sum"<<mul_sum<<endl;
            avg = mul_sum/sum;
            buffer_gray[i*vWidth+j] = mul_sum;
           }
    }


}

void gradient()
{
	int i,j,m,n,ext = WINDOW_SIZE/2;
	double grad_x=0,grad_y=0;
	unsigned char window[WINDOW_SIZE][WINDOW_SIZE];

	for(i = ext;i< (vHeight-ext);i++)
	{
	       for(j = HalfWidth+ext; j< (vWidth-ext);j++)
	       {
	            //form a window
	            for(m=0;m<WINDOW_SIZE;m++)
	            {
	                for(n=0;n<WINDOW_SIZE;n++)
	                {
	                 window[m][n] = buffer_gray[(i-ext+m)*vWidth+(j-ext+n)];
	                }
	            }
	            //sobel filter the window:
	            grad_x = ((window[0][2]+(2*window[1][2])+window[2][2])-(window[0][0]+(2*window[1][0])+window[2][0]));
	            grad_y = ((window[0][0]+(2*window[0][1])+window[0][2])-(window[2][0]+(2*window[2][1])+window[2][2]));
	            //cout<<sqrt(pow(grad_x,2)+pow(grad_y,2))<<endl;
	            gradient_mag[i*vWidth+j] = (unsigned char) sqrt(pow(grad_x,2)+pow(grad_y,2));
	            //cout<<(int)gradient_mag[i*vWidth+j]<<endl;

	            if(grad_x!=0)
	            	gradient_direc[i*vWidth+j] = (atan(grad_y/grad_x))*180*7/22; // in degree

	            else
	            {
	            	if(grad_y==0)
	            		gradient_direc[i*vWidth+j] = (unsigned char)0;
	            	else
	            		gradient_direc[i*vWidth+j] = (unsigned char)90;
	            }
	            if(gradient_direc[i*vWidth+j]<=(unsigned char)10)
	            	gradient_direc[i*vWidth+j] = (unsigned char)0;
	            else if(gradient_direc[i*vWidth+j]<=(unsigned char)60)
	            	gradient_direc[i*vWidth+j] = (unsigned char)45;
	            else if(gradient_direc[i*vWidth+j]<=(unsigned char)110)
	            	gradient_direc[i*vWidth+j] = (unsigned char)90;
	            else
	            	gradient_direc[i*vWidth+j] = (unsigned char)135;

	            //cout<<gradient_direc[i*vWidth+j]<<endl;
	       }
	}
}

void calc_poss_edges()
{
	int i,j;
	for(i=0;i<vHeight;i++)
	{
		for(j=HalfWidth;j<vWidth;j++)
		{
			//cout<<(int)gradient_direc[i*vWidth+j]<<endl;
			switch((int)gradient_direc[i*vWidth+j])
			{
				case 0:
					if((gradient_mag[i*vWidth+j]>gradient_mag[i*vWidth+(j-1)]) && (gradient_mag[i*vWidth+j]>gradient_mag[i*vWidth+(j+1)]))
					{
						poss_edge[i*vWidth+j] = 1;
						dif_total++;

					}
					break;
				case 45:
					if((gradient_mag[i*vWidth+j]>gradient_mag[(i-1)*vWidth+(j+1)]) && (gradient_mag[i*vWidth+j]>gradient_mag[(i+1)*vWidth+(j-1)]))
					{
						poss_edge[i*vWidth+j] = 1;
						dif_total++;

					}
					break;
				case 90:
					if((gradient_mag[i*vWidth+j]>gradient_mag[(i-1)*vWidth+j]) && (gradient_mag[i*vWidth+j]>gradient_mag[(i+1)*vWidth+j]))
					{
						poss_edge[i*vWidth+j] = 1;
						dif_total ++;

					}
					break;
				case 135:
					if((gradient_mag[i*vWidth+j]>gradient_mag[(i-1)*vWidth+(j-1)]) && (gradient_mag[i*vWidth+j]>gradient_mag[(i+1)*vWidth+(j+1)]))
					{
						poss_edge[i*vWidth+j] = 1;
						dif_total++;

					}
					break;

			}
		}
	}

}

int histogram()
{
	int i,j,val;

    //FILE *file;
    //initialize histogram to all zeros.
    for (i = 0; i < 256; i++)
    {
        count_hist[i] = 0;
    }
    for(i = 0;i<vHeight;i++)
    {
        for(j = HalfWidth;j<vWidth;j++)
        {
                    val = gradient_mag[i*vWidth+j];
                    count_hist[val] +=1;
        }
    }

    return 0;
}

void calc_thresholds(int amt_edgeinfo,int perimeter,int Th_max)
{

	int x;

	//int formula_array[256];
	Th_h = 0;
	Th_l = 0;

	//printf("Hey\n");
	//printf("Hey\n");
	//printf("%d\n",xtra);
	formula_array[0] = 0;
	for(x=1;x<256-1;x++)
	{
		//printf("%d\n",x);
		if(((diff[x-1]+diff[x])>0) || ((diff[x] + diff[x+1])>0))
		{
			formula_array[x] = 1;
		}
		else
		{
			formula_array[x] = 0;
		}
		//formula_array[i] = ((((diff[i-1]+diff[i])>0) || ((diff[i] + diff[i+1])>0))?1:0);
		//printf("%d\n",formula_array[x]);*/
	}
	//printf("k");
	formula_array[x] = (((diff[x-1]+diff[x])>0)?1:0);
	//printf("k");

	//printf("%d %d\n",amt_edgeinfo,perimeter);
	if(amt_edgeinfo < 5*perimeter) // little edge information
	{
		little_edge_info_th(Th_max);
	}

	else if(amt_edgeinfo >= 5*perimeter || amt_edgeinfo < 200*perimeter)  //rich edge information image
	{
		rich_edge_info_th(perimeter,Th_max);
	}

	else if(amt_edgeinfo >= 200*perimeter)
	{
		numerous_edge_info_th(Th_max);
	}

}

void little_edge_info_th(int Th_max)
{
	int i,j = 0;
	//int high_thresh_poss[256] ={0};
	for(i=0;i<=Th_max;i++)
	{
		if(formula_array[i] == 1)
		{
			 high_thresh_poss[j] = i;
			 j++;
		}
	}

//max of high_thres_poss
	Th_h = 0;
	for(i=0;i<256;i++)
	{
		if(high_thresh_poss[i]>Th_h)
			Th_h = high_thresh_poss[i];
	}

	    Th_l = Th_h/2;
	    //printf("%d\n%d",Th_h,Th_l);
}

void rich_edge_info_th(int perimeter,int Th_max)
{
	int i,j = 0;
	int dif_5C = 5*perimeter;
	int s_high,s_low,exit_cond1,exit_cond2;
	int dif_th;
	//int high_thresh_poss[256-3]={0};//,low_thresh_poss[256-3]={0};
	//int htp[256-3]={0},ltp[256-3]={0};

	//printf("in\n");
	for(i = 3;i<=Th_max;i++)
	{
	    if((int)formula_array[i] == 1)
	    {

	        high_thresh_poss[j] = i;
			j++;
	    }
	    else
		{
	    	high_thresh_poss[j] = 0;
			j++;
		}
	    //printf("%d\n",i);
	}

	s_high = j-1;
	s_low = j-1;

	j=0;
	for(i=0;i<s_high;i++)
	{
		if((int)high_thresh_poss[i]!=0)
		{
			htp[j] = high_thresh_poss[i];
			ltp[j] = high_thresh_poss[i];
			j++;
		}
	}

	s_high = j-1;
	s_low = j-1;

	//qsort(high_thresh_poss, s_high, sizeof(int), cmpfunc);
	//qsort(low_thresh_poss, s_low, sizeof(int), cmpfunc);

	Th_h = (int)htp[s_high];
	Th_l = (int)ltp[s_low];
	exit_cond1 = 0;
	exit_cond2 = 0;

	while(exit_cond1==0 || exit_cond2==0)
	{        //    while(cnt~=10)
	        dif_th = 0;
			//cout<<Th_h<<" "<<Th_l<<endl;
	        for(i=0;i<vHeight;i++)
	        {
	            for (j=HalfWidth;j<vWidth;j++)
	            {
	                if(((int)gradient_mag[i*vWidth+j] > Th_h) || ((int)gradient_mag[i*vWidth+j] >Th_l))
					{
	                    dif_th += (((int)buffer_gray[i*vWidth+j]+255)/255);
						//dif_th++;
		//				cout<<dif_th<<endl;

					}
	            }
	        }
			//cout<<dif_th<<" "<<dif_5C<<" "<<dif_total/2<<endl;
	        //printf("%d %d %d\n",dif_th,dif_5C,(dif_total/2));

			//cout<<s_high<<" "<<s_low<<endl;
	        if (dif_th>dif_5C)
	        {
	        	exit_cond1 =1;
				//cout<<"in"<<endl;
	        }
	        else
	        {
	            s_high= s_high-1;
	        }

	        if (dif_th>dif_total/2)
	        {
	            exit_cond2 = 1;
				//cout<<"out"<<endl;
	        }
	        else
	        {
	            s_low = s_low-1;
	        }
			//if(s_high <0)
				//s_high = 256-3;
			//if(s_low <0)
				//s_low = 256-3;
	        //qsort(htp, s_high, sizeof(int), cmpfunc);
	        //qsort(ltp, s_low, sizeof(int), cmpfunc);
	        Th_h = (int)htp[s_high];

	        Th_l = (int)ltp[s_low];
	    	//printf("%d %d\n",Th_h,Th_l);

	}
}

void numerous_edge_info_th(int Th_max)
{
	int i,j = 0;

	int s_high,s_low,exit_cond1,exit_cond2;
	int dif_th;
	//int high_thresh_poss[256-3]={0};//,low_thresh_poss[256-3]={0};
	//int htp[256-3]={0},ltp[256-3]={0};

	//printf("in\n");
	for(i = 3;i<=Th_max;i++)
	{
	    if((int)formula_array[i] == 1)
	    {

	        high_thresh_poss[j] = i;
			j++;
	    }
	    else
		{
	    	high_thresh_poss[j] = 0;
			j++;
		}
	    //printf("%d\n",i);
	}

	s_high = j-1;
	s_low = j-1;

	j=0;
	for(i=0;i<s_high;i++)
	{
		if((int)high_thresh_poss[i]!=0)
		{
			htp[j] = high_thresh_poss[i];
			ltp[j] = high_thresh_poss[i];
			j++;
		}
	}

	s_high = j-1;
	s_low = j-1;

	//qsort(high_thresh_poss, s_high, sizeof(int), cmpfunc);
	//qsort(low_thresh_poss, s_low, sizeof(int), cmpfunc);

	Th_h = (int)htp[s_high];
	Th_l = (int)ltp[s_low];
	exit_cond1 = 0;
	exit_cond2 = 0;

	while(exit_cond1==0 || exit_cond2==0)
	{        //    while(cnt~=10)
	        dif_th = 0;
			//cout<<Th_h<<" "<<Th_l<<endl;
	        for(i=0;i<vHeight;i++)
	        {
	            for (j=HalfWidth;j<vWidth;j++)
	            {
	                if(((int)gradient_mag[i*vWidth+j] > Th_h) || ((int)gradient_mag[i*vWidth+j] >Th_l))
					{
	                    dif_th += (((int)buffer_gray[i*vWidth+j]+255)/255);
						//dif_th++;
		//				cout<<dif_th<<endl;

					}
	            }
	        }
			//cout<<dif_th<<" "<<dif_5C<<" "<<dif_total/2<<endl;
	        //printf("%d %d %d\n",dif_th,dif_5C,(dif_total/2));

			//cout<<s_high<<" "<<s_low<<endl;
	        if (dif_th>dif_total/12)
	        {
	        	exit_cond1 =1;
				//cout<<"in"<<endl;
	        }
	        else
	        {
	            s_high= s_high-1;
	        }

	        if (dif_th>dif_total/4)
	        {
	            exit_cond2 = 1;
				//cout<<"out"<<endl;
	        }
	        else
	        {
	            s_low = s_low-1;
	        }
			//if(s_high <0)
				//s_high = 256-3;
			//if(s_low <0)
				//s_low = 256-3;
	        //qsort(htp, s_high, sizeof(int), cmpfunc);
	        //qsort(ltp, s_low, sizeof(int), cmpfunc);
	        Th_h = (int)htp[s_high];

	        Th_l = (int)ltp[s_low];
	    	//printf("%d %d\n",Th_h,Th_l);

	}
}

void edge_mapping()
{
	int i,j;
	//printf("in\n");
	for (i=1;i<vHeight-1;i++)
	{
	    for (j=HalfWidth+1;j<vWidth-1;j++)
	    {
	        if(gradient_mag[i*vWidth+j] > Th_h && poss_edge[i*vWidth+j]==1)
	        {
	            edge_map[i*vWidth+j] = 1;
	            switch(gradient_direc[i*vWidth+j])
	            {
	                case 0:
	                    if(gradient_mag[i*vWidth+(j-1)]>Th_l &&poss_edge[i*vWidth+(j-1)]==1)
	                        edge_map[i*vWidth+(j-1)]=1;

	                    if(gradient_mag[i*vWidth+(j+1)]>Th_l&&poss_edge[i*vWidth+(j+1)]==1)
	                        edge_map[i*vWidth+(j+1)]=1;
	                    break;
	                case 45:
	                    if(gradient_mag[(i-1)*vWidth+(j+1)]>Th_l&&poss_edge[(i-1)*vWidth+(j+1)]==1)
	                        edge_map[(i-1)*vWidth+(j+1)]=1;

	                    if(gradient_mag[(i+1)*vWidth+(j-1)]>Th_l&&poss_edge[(i+1)*vWidth+(j-1)]==1)
	                        edge_map[(i+1)*vWidth+(j-1)]=1;
	                    break;
	                case 90:
	                    if(gradient_mag[i*vWidth+(j+1)]>Th_l&&poss_edge[i*vWidth+(j+1)]==1)
	                        edge_map[i*vWidth+(j+1)]=1;

	                    if(gradient_mag[i*vWidth+(j-1)]>Th_l&&poss_edge[i*vWidth+(j-1)]==1)
	                        edge_map[i*vWidth+(j-1)]=1;
	                    break;
	                case 135:
	                    if(gradient_mag[(i-1)*vWidth+(j-1)]>Th_l&&poss_edge[(i-1)*vWidth+(j-1)]==1)
	                        edge_map[(i-1)*vWidth+(j-1)]=1;

	                    if(gradient_mag[(i+1)*vWidth+(j+1)]>Th_l&&poss_edge[(i+1)*vWidth+(j+1)]==1)
	                        edge_map[(i+1)*vWidth+(j+1)]=1;
	                    break;
	            }

	        }
	    }
	}

}

void sketch()
{
	int i,j;
	for(i=0;i<vHeight;i++)
	{
		for(j=HalfWidth;j<vWidth;j++)
		{
			if(edge_map[i*vWidth+j] == 1 && poss_edge[i*vWidth+j] ==1)
				edge_map[i*vWidth+j] = buffer_gray[i*vWidth+j];
			else
				edge_map[i*vWidth+j] = 255;
		}
	}
}

