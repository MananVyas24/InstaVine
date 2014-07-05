/*
 * header_file.h
 *
 *  Created on: Apr 22, 2014
 *      Author: Shruti
 */

#ifndef HEADER_FILE_H_
#define HEADER_FILE_H_
#include <stdlib.h>
#include <math.h>
#include "evmdm6437.h"
#include "evmdm6437_dip.h"
#include "evmdm6437_led.h"

extern Int16 video_loopback_test();

// size for buffer_in: 720 * 480 / 2, the reason is explained below.
#define Pixels 172800

// Resolution 720 * 480 (NTSC mode)
#define vWidth 720
#define vHeight 480
#define HalfWidth 360
#define HalfHeight 240

// CAN change the internal block size here, the example is 60 * 120
#define INTERNAL_BLK_vWidth 60
#define INTERNAL_BLK_vHeight 120
#define WINDOW_SIZE 3
#define MAXI 256
#define MAXP 240
#define MINI 16
#define WB_FRAMES 4
#define WB_fpow 2

// Define a space on memory for save the information input and output (Interface data)

extern Uint32 buffer_out[Pixels]; //from 0x80000000
extern Uint32 buffer_in[Pixels]; //from 0x800A8C00, which is the same as 4 (bytes for integer) * Pixels
extern Uint32 buffer_proc1[Pixels]; //from 0x80151800
extern Uint32 buffer_proc2[Pixels];
extern Uint32 logo[4864];
extern Uint32 logo_small[1216];

//for edge detection:
extern unsigned char buffer_gray[Pixels*2];
extern unsigned char gradient_mag[Pixels*2];
extern unsigned char gradient_direc[Pixels*2];
extern unsigned char poss_edge[Pixels*2];
extern unsigned char edge_map[Pixels*2];
extern int count_hist[256];
extern int pixel_val[256];
extern int diff[256];
extern unsigned char formula_array[256];
extern unsigned char high_thresh_poss[256];
extern unsigned char htp[256];
extern unsigned char ltp[256];
//extern unsigned char filtered[Pixels*2];

//for vignette:
extern Uint32 buffer_proc_inter[Pixels];
extern unsigned char r[Pixels*2];
extern unsigned char g[Pixels*2];
extern unsigned char b[Pixels*2];
extern unsigned char Cb[Pixels*2];
extern unsigned char Cr[Pixels*2];
extern unsigned char Y[Pixels*2];
extern int hue[Pixels*2];
extern unsigned char segment[Pixels*2];
extern int density[vWidth];

//for cartoonize:
extern double gaussian_kernel[256];

//for comic book:
extern unsigned char OutputImageR[Pixels*2];
extern unsigned char OutputImageG[Pixels*2];
extern unsigned char OutputImageB[Pixels*2];


//for edge detection:
extern double Gauss_norm[WINDOW_SIZE][WINDOW_SIZE];
extern int dif_total;
extern int Th_h;
extern int Th_l;

//for vignette
extern int center_row;
extern int center_col;
extern int leftmost;
extern int rightmost;
extern int bottommost;
extern int topmost;

//for cartoonize:
extern double WindowArray[17][17];


//for edge detection:
void grayscale(void);
void gaussian(int sigma);
void gaussian_filter();
void gradient();
void calc_poss_edges();
extern int histogram();
void calc_thresholds(int amt_edgeinfo,int perimeter,int Th_max);
void little_edge_info_th(int Th_max);
void rich_edge_info_th(int perimeter,int Th_max);
void numerous_edge_info_th(int Th_max);
void edge_mapping();
void sketch();
void pencil_sketch();

//for vignette:
void vignette_effect();
void ycbcr_to_rgb_half();
void ycbcr_to_rgb_quarter();
void rgb_to_hsv_quarter();
void skin_segmentation();
void focal_point();
void fade();
void rgb_to_ycbcr_half();
void rgb_to_ycbcr_quarter();

//for cartoonize:
void cartoonize();
void bilateral();

//for emboss:
void emboss_effect();
void emboss();

//for comic book:
void comic_book();
void sharpen_edges();

//for grayscale vignette:
void vignette_gray();
void fade_gray();

//for white balance & color changes
extern int histograms[WB_FRAMES][MAXI][3];
void White_balance();// unsigned char *r, unsigned char *g, unsigned char *b);
void color_temp( );//unsigned char *r,  unsigned char *g,  unsigned char *b, int temperature);
void Init_histo();
void sepia();// unsigned char *r,  unsigned char *g,  unsigned char *b);
void Shift_histos();
void diffuse();
#endif /* HEADER_FILE_H_ */
