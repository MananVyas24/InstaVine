#include "White_balance.h"

int histograms[WB_FRAMES][MAXI][3];


void Init_histo(){
	int j,i;
	for (j = 0; j < MAXI; j++){
		for (i = 0; i < WB_FRAMES; i++){
			histograms[i][j][0] = 0;
			histograms[i][j][1] = 0;
			histograms[i][j][2] = 0;

		}
	}
}


void Shift_histos(){
	int i,j;
	for (i = 1; i < WB_FRAMES-1; i++){
		memcpy(histograms + (i - 1)*MAXI*3, histograms+i*MAXI*3, MAXI*3); //shift histograms down an index
	}
	for (j = 0; j < MAXI; j++){
		histograms[WB_FRAMES - 1][j][0] = 0;
		histograms[WB_FRAMES - 1][j][1] = 0;
		histograms[WB_FRAMES - 1][j][2] = 0;

	}
}

void White_balance(unsigned char *r,unsigned char *g,unsigned char *b){
	unsigned int avg_hist[MAXI][3] = {0};
	Shift_histos();
	unsigned int bsum[3] = {0,0,0}, tsum[3] = {0,0,0};
	unsigned int indexb[3], indext[3];
	int i,j,k;
	int tr, tg, tb;
	for (i = 0; i < TPixels; i++){
		histograms[WB_FRAMES - 1][r[i]][0]++;//= r[i];
		histograms[WB_FRAMES - 1][g[i]][1]++;//= g[i];
		histograms[WB_FRAMES - 1][b[i]][2]++;//= b[i];
	}

	for (i = 0; i < MAXI; i++){
		for (j = 0; j < WB_FRAMES; j++){
			avg_hist[i][0] += histograms[j][i][0]>>WB_fpow;
			avg_hist[i][1] += histograms[j][i][1]>>WB_fpow;
			avg_hist[i][2] += histograms[j][i][2]>>WB_fpow;
		}
		//avg_hist[i][0]=avg_hist[i][0] >> WB_fpow;
		//avg_hist[i][1]=avg_hist[i][1] >> WB_fpow;
		//avg_hist[i][2]=avg_hist[i][2] >> WB_fpow;
		for (k = 0; k < 3; k++){
			if (bsum[k] <= Pixels >> 6){
				bsum[k] += avg_hist[i][k];
				indexb[k] = i;
			}
		}
	}
	for (k = 0; k < 3; k++){
		for (i = MAXI - 1; i > 0; i--){
			if (tsum[k] <= Pixels >> 6){
				tsum[k] += avg_hist[i][k];
				indext[k] = i;
			}
		}
	}
	unsigned char scale[MAXI][3];
	for (i = 0; i < MAXI; i++){
		for(k = 0; k< 3; k++){
			scale[i][k] = i < indexb[k] ? MINP : i>indext[k] ? MAXP : (i-indexb[k])*(MAXP-MINP)/(indext[k]-indexb[k]);
		}	
	}

	for (i = 0; i < TPixels; i++){

		r[i] = scale[r[i]][0];
		g[i] = scale[g[i]][1];
		b[i] = scale[b[i]][2];

	}
}

