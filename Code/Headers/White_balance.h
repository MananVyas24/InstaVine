#define MAXI 256
#define WB_FRAMES 4
#define WB_fpow 2

int histograms[WB_FRAMES][MAXI][3];
int temp_hist[256][3];

void White_balance();
void Init_histo();
void Shift_histos();
