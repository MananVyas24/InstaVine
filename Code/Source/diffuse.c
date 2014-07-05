#include "header_file.h"

void diffuse(){
	int i, j, x,y, hd, dstx,dsty, m=0;
	unsigned char tempr, tempg, tempb;
	static int dif_dist=0;
	ycbcr_to_rgb_half();
	dif_dist = dif_dist > 50? dif_dist = 1 : dif_dist + 5;
	hd = dif_dist/2;
	for (i = 0; i < vHeight; i++){
		for(j = HalfWidth; j < vWidth; j++){
			x = rand()%dif_dist - hd;
			y = rand()%dif_dist - hd;
			
			dstx = j+x <HalfWidth ? HalfWidth : j+ x >= vWidth ? vWidth:j +x;
			dsty = i+y <0 ? 0 : i+y >= vHeight ? vHeight :i +y;
			
			//swap pixels from randomly generated locations
			tempr = r[dstx+dsty*vWidth];
			tempg = g[dstx+dsty*vWidth];
			tempb = b[dstx+dsty*vWidth];
			OutputImageR[dsty*vWidth +dstx] = r[i*vWidth+j];
			OutputImageG[dsty*vWidth +dstx] = g[i*vWidth+j];
			OutputImageB[dsty*vWidth +dstx] = b[i*vWidth+j];
			OutputImageR[i*vWidth+j] = tempr;
			OutputImageG[i*vWidth+j] = tempg;
			OutputImageB[i*vWidth+j] = tempb;
		}
	}
	for (i = 0; i < vHeight; i++){
		for(j = HalfWidth; j < vWidth; j++){
			r[i*vWidth+j] = OutputImageR[i*vWidth+j];
			g[i*vWidth+j] = OutputImageG[i*vWidth+j];
			b[i*vWidth+j] = OutputImageB[i*vWidth+j];
		}
	}	
	
	rgb_to_ycbcr_half();
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