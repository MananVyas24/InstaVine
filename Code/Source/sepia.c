#include "White_balance.h"
void sepia(unsigned char *r, unsigned char * g, unsigned char * b){
	int i, tempr, tempb, tempg, gray;
	float temp;
	unsigned char gamma[MAXI] = {0.0};
	for(i = 0; i < MAXI; i++){
		temp = i / 255.0;
		temp = pow(temp, 0.625); //gamma of 1.6
		gamma[i] = temp > 255 ? 255 : temp < 0 ? 0: (unsigned char)(temp * 255); //gamma lookup table
	}
	for (i = 0; i < TPixels; i ++){
		tempr = (int)r[i];
		tempg = (int)g[i];
		tempb = (int)b[i];
		gray = (tempr + tempg + tempb) / 3;
		tempr = (tempr + gray) >> 1;
		tempg = (tempg + gray) >> 1;
		tempb = (tempb + gray) >> 1;
		
		//mix color channels
		tempr = (int)(1.75*(tempg * tempb) / 255);
		tempg = (int)(1.75*(tempb * tempr) / 255);
		tempb = (int)(1.75*(tempr * tempg) / 255);
		tempr = tempr > 255 ? 255: tempr < 0 ? 0: tempr;
		tempg = tempg > 255 ? 255: tempg < 0 ? 0: tempg;
		tempb = tempb > 255 ? 255: tempb < 0 ? 0: tempb;
		r[i] = gamma[tempr];
		g[i] = gamma[tempg];
		b[i] = gamma[tempb];
	}	
}