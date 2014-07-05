

void Init_histo(){
	for (int j = 0; j < MAXI; j++){
		for (int i = 0; i < WB_FRAMES; i++){
			histograms[i][j][0] = 0;
			histograms[i][j][1] = 0;
			histograms[i][j][2] = 0;

		}
		temp_hist[j][0] = 0;
		temp_hist[j][1] = 0;
		temp_hist[j][2] = 0;
	}
}