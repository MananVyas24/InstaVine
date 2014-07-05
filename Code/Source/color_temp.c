#include "White_balance.h"

#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))

void color_temp(unsigned char *r, unsigned char *g, unsigned char *b, int temperature){
	static int framecnt = 0;
	int mytemp=0; 
	mytemp = temperature < 1000 ? 10 : (temperature > 40000 ? 400 : temperature / 100);
 	int  tr, tg, tb, tk, i;
	// set temp values for the passed color temperature
	if (mytemp <= 66){
		tr = MAXP;
		tk = 99.4708025861 * log(mytemp) - 161.1195681661;
		tg = tk < MINP ? MINP : tk > MAXP ? MAXP : tk;
		if (mytemp <= 19) tb = 0;	
		else{
			tk = 138.5177312231 * log(mytemp-10) - 305.0447927307;
			tb = tk < MINP ? MINP : tk > MAXP ? MAXP : tk;
		}
	}
	else{
		tb = 255;
		tr = 323 * pow(mytemp-60, -0.1332047592);
		tr = tr < MINP ? MINP : tr> MAXP ? MAXP : tr;

		tk = 288.1221695283 * pow(mytemp-60, -0.0755148492);
		tg = tk < MINP ? MINP : tk > MAXP ? MAXP : tk;

	}
			
	int tempr, tempg, tempb, h, s, lum, lumi;
	int min, max, d, tr2, tg2, tb2, p, q;
		tr2 = (tr<<13)/255; // <<13 = * 8192
		tg2 = (tg<<13)/255;
		tb2 = (tb<<13)/255;
	float df, sf, hf, oddf, temprf, tempgf, tempbf; 
	for(i = 0; i < TPixels; i++){
		tempr = (r[i]<<13)/255;
		tempg = (g[i]<<13)/255;
		tempb = (b[i]<<13)/255;
		

		//blend original rgb values with the rgb of the temperature setting
		min = MIN(tempr, MIN(tempg, tempb));
		max = MAX(tempr, MAX(tempg, tempb));
		//printf("%f, %f\n", min, max);
		lumi = (max + min) >> 1;
			

		tempr = ( tr2+ tempr) >> 1;
		tempg = ( tg2+ tempg) >> 1;
		tempb = ( tb2+ tempb) >> 1;
		

		//convert to HSL space
		min = MIN(tempr, MIN(tempg, tempb));
		max = MAX(tempr, MAX(tempg, tempb));
		lum = (max + min) >> 1;
		if (min == max) {
			h = 0;
			s = 0;
			framecnt++;
			//if (framecnt > 3) printf("same\n");
		}
		else{
			d = max-min;
			s = lum <= 4096 ? ((d / ((max+min)>>7))<<6) : ((d / ((16384-max-min)>>7))<<6);
			if (framecnt > 3 && s > 0) printf("%d\n",i);
			if(max == tempr) h =  ((tempg - tempb) / (d>>7))<<6;
			else if (max == tempg) h =  (((tempb - tempr) / (d >> 7))<<6) + 16384;
			else  h =  (((tempr - tempg) / (d >> 7))<<6) + 32768;			
		}

		/*else{
			d = (max - min);
			df = (float)(max-min)/8192.0;
			oddf = (float)(max+min)/8192.0;
			temprf = (float)tempr/8192.0;
			tempbf = (float)tempb/8192.0;
			tempgf = (float)tempg/8192.0;
			sf = lum <= 4096 ? df / oddf : df / (2.0-df);
			if(max == tempr) hf =  (tempgf - tempbf) / df; // + (tempg < tempb ? 6.0 : 0.0); 
			else if (max == tempg) hf =  (tempbf - temprf) / df + 2;
			else  hf =  (temprf - tempgf) / df + 4;	
			s = (int)(sf * 8192);	
			h = (int)(hf * 8192);	
		}*/
		//switch back to rgb but replace with original luminance
		if ((s) == 0) r[i] = g[i] = b[i] = (unsigned char)((lumi*255)>>13);
		else{
			//framecnt++;
			//if (framecnt > 3) printf("%d\n",i);
			q = lumi <= 4096 ? (lumi * (8192 - s)) >> 13 : lumi - ((s * (8196-lumi)) >> 13);
			p =  (lumi<<1) - q;
			if( h < 8196){
				tempr = p;
				if(h < 0){
				tempg = q;
				tempb = tempg - ((h * (p-q))>>13);	
				}
				else {
					tempb = q;
					tempg = ((h * (p - q))>>13) + tempb;
				}
			}
			else if( h < 24576){
				tempg = p;
				if ( h < 16384){
					tempb = q;
					tempr = tempb - (((h-16384) * (p-q))>>13);
				}
				else{
				tempr = q;
				tempb = (((h-16384) * (p-q))>>13) + tempr;
				}
			}
			else{
			tempb = p;
			if( h < 32768){
			tempr = q;
			tempg = tempr - (((h-32768) * (p-q))>>13);
			}
			else{
				tempg = q;
				tempr = (((h-32768) * (p-q))>>13) + tempg;
			}	
			}
	
		r[i] = tempr < 0 ? MINP : tempr > 7710 ? MAXP : (unsigned char)((tempr*255)>>13);
		g[i] = tempg < 0 ? MINP : tempg > 7710 ? MAXP : (unsigned char)((tempg*255)>>13);
		b[i] = tempb < 0 ? MINP : tempb > 7710 ? MAXP : (unsigned char)((tempb*255)>>13);
		}
	}
	/*float min,max,d, tr2,tg2,tb2,p,q;
	float tempr, tempg, tempb, h, s, lum, lumi;
	
	tr2 = (float)tr/255.0;
	tg2 = (float)tg/255.0;
	tb2 = (float)tb/255.0;
	
	for (i = 0; i < TPixels; i++){
		tempr = (float)r[i]/255.0;
		tempg = (float)g[i]/255.0;
		tempb = (float)b[i]/255.0;
		//blend original rgb values with the rgb of the temperature setting
		min = MIN(tempr, MIN(tempg, tempb));
		max = MAX(tempr, MAX(tempg, tempb));
		lumi = (max + min) / 2.0; //original pixel luminance

		tempr = ( tr2+ tempr) / 2.0;
		tempg = ( tg2+ tempg) / 2.0;
		tempb = ( tb2+ tempb) / 2.0; 
		
		//convert to HSL space
		min = MIN(tempr, MIN(tempg, tempb));
		max = MAX(tempr, MAX(tempg, tempb));
		lum = (max + min) / 2.0;
		if ((int)(min*100) == (int)(max*100)) {
			h = 0.0;
			s = 0.0;
		}
		else{
			d = max-min;
			s = lum <= 0.5 ? d / ( max+min) : d / (2-max-min);
			if(max == tempr) h =  (tempg - tempb) / d; // + (tempg < tempb ? 6.0 : 0.0); 
			else if (max == tempg) h =  (tempb - tempr) / d + 2.0;
			else  h =  (tempr - tempg) / d + 4.0;			
		}
		
		//switch back to rgb but replace with original luminance
		if ((int)(100*s) == 0) r[i] = g[i] = b[i] = (unsigned char)(lumi*255);
		else{
			q = lumi <= 0.5 ? lumi * (1.0 - s) : lumi - s * (1-lumi);
			p = 2.0 * lumi - q;
			if( h < 1.0){
				tempr = p;
				if(h < 0.0){
				tempg = q;
				tempb = tempg -h * (p-q);	
				}
				else {
					tempb = q;
					tempg = h * (p - q) + tempb;
				}
			}
			else if( h < 3.0){
				tempg = p;
				if ( h < 2.0){
					tempb = q;
					tempr = tempb - (h-2.0) * (p-q);
				}
				else{
				tempr = q;
				tempb = (h-2.0) * (p-q) + tempr;
				}
			}
			else{
			tempb = p;
			if( h < 4.0){
			tempr = q;
			tempg = tempr - (h-4.0) * (p-q);
			}
			else{
				tempg = q;
				tempr = (h-4.0) * (p-q) + tempg;
			}	
			}
	
		r[i] = tempr < 0.0 ? MINP : tempr > 1.0 ? MAXP : (unsigned char)(tempr*255);
		g[i] = tempg < 0.0 ? MINP : tempg > 1.0 ? MAXP : (unsigned char)(tempg*255);
		b[i] = tempb < 0.0 ? MINP : tempb > 1.0 ? MAXP : (unsigned char)(tempb*255);
		}
	}*/
}
