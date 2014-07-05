#include "header_file.h"
void emboss_effect(void)
{
	int i,j,m=0;

	//ycbcr to rgb
	ycbcr_to_rgb_half();

	//emboss
	emboss();

	//rgb to ycbcr
	rgb_to_ycbcr_half();

	//transfer to buffer proc 2 so that it can be moved to output buffer
	for(i=0; i<vHeight; i++)
	{
		for (j=0; j<vWidth/2; j++)
		{
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

					buffer_proc2[i*vWidth/2+j] = buffer_proc_inter[i*vWidth/2+j];
				}

			}
		}
	}

}

void emboss(void)
{
	int red = 0;
	int green = 0;
	int blue = 0;
	int ii;	int jj; int h; int l;
	int R; int G; int B;
	int factor = 1; // FACTOR = 3 ; BIAS = 1 GIVES CARTOONISH EFFECT
	int bias = 128;
	int Emboss_Mask[5][5] = {
		                   	{-1, -1, -1, -1, 0},
	                      	{-1, -1, -1,  0, 1},
	                       	{-1, -1,  0,  1, 1},
	                       	{-1,  0,  1,  1, 1},
	                       	{ 0,  1,  1,  1, 1},
	               	   };


	// -------------------------- EMBOSS FILTER ---------------------------------

   	for (ii=1; ii<vHeight-1; ii++)
    	{
        	for (jj=1+HalfWidth; jj<vWidth-1; jj++)
        	{
            	red = 0;
            	green = 0;
            	blue = 0;

           		for (h=-2; h<=2; h++)
            	{
                	for (l=-2; l<=2; l++)
                	{
                    	//int imageX = (i-3/2+h+vHeight) % vHeight;
                    	//printf ("LocnX : %d\n",((ii+h)*vWidth)+(jj+k));
                    	//int imageY = (j-3/2+k+vWidth) % vWidth;
                    	//printf ("  LocnY : %d\n",imageY);
                    	red += r[((ii+h)*vWidth)+(jj+l)] * Emboss_Mask[h+2][l+2];
                    	green += g[((ii+h)*vWidth)+(jj+l)] * Emboss_Mask[h+2][l+2];
                    	blue += b[((ii+h)*vWidth)+(jj+l)] * Emboss_Mask[h+2][l+2];
                	}
            	}

            	R = (factor)*red + (bias);
            	G = (factor)*green + (bias);
            	B = (factor)*blue + (bias);

            	if (R<16) {R=16;}
            	else if (R>240) {R=240;}

            	if (G<16) {G=16;}
            	else if (G>240) {G=240;}

            	if (B<16) {B=16;}
            	else if (B>240) {B=240;}

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
}
