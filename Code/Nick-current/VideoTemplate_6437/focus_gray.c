#include "header_file.h"
void vignette_gray(void)
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
	fade_gray();

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
void fade_gray(void)
{
	int radius,grade,i,j;
	int center_r,center_c;
	double L,f,Ln;

	//Parameters: change these to change the size of the vignette
	radius = 1;
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
	    	r[i*vWidth+j] = (unsigned char)(f*(int)r[i*vWidth+j]+ (1-f)*Y[i*vWidth+j]);
	    	g[i*vWidth+j] = (unsigned char)(f*(int)g[i*vWidth+j]+ (1-f)*Y[i*vWidth+j]);
	    	b[i*vWidth+j] = (unsigned char)(f*(int)b[i*vWidth+j]+ (1-f)*Y[i*vWidth+j]);
	    }
	  }
	}
}
