#include "header_file.h"
void sepia(){//unsigned char *r, unsigned char * g, unsigned char * b){
	ycbcr_to_rgb_half();
	int i,j, tempr, tempb, tempg, gray, m=0;
	float temp;
	unsigned char gamma[MAXI] = {0};
	for(i = 0; i < MAXI; i++){
		temp = i / 255.0;
		temp = pow(temp, 0.625); //gamma of 1.6
		gamma[i] = temp > 255 ? 255 : temp < 0 ? 0: (unsigned char)(temp * 255); //gamma lookup table
	}
	for (i = 0; i < vHeight; i ++){
		for(j = HalfWidth; j<vWidth; j++){
			tempr = (int)r[i*vWidth + j];
			tempg = (int)g[i*vWidth + j];
			tempb = (int)b[i*vWidth + j];
			gray = (tempr + tempg + tempb) / 3;
			tempr = (tempr + gray) >> 1;
			tempg = (tempg + gray) >> 1;
			tempb = (tempb + gray) >> 1;
			
			//mix color channels
			tempr = (int)(7*(tempg * tempb) / 1020); // equal to 1.75* xx / 255
			tempg = (int)(7*(tempb * tempr) / 1020);
			tempb = (int)(7*(tempr * tempg) / 1020);
			tempr = tempr > 255 ? 255: tempr < 0 ? 0: tempr;
			tempg = tempg > 255 ? 255: tempg < 0 ? 0: tempg;
			tempb = tempb > 255 ? 255: tempb < 0 ? 0: tempb;
			r[i*vWidth + j] = gamma[tempr];
			g[i*vWidth + j] = gamma[tempg];
			b[i*vWidth + j] = gamma[tempb];
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