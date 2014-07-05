#include "header_file.h"
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
	int y0,cb,cr,y1,r_temp1,g_temp1,b_temp1,r_temp2,g_temp2;

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

void focal_point()
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



