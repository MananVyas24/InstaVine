#ifndef TEXT_FUNCTIONS_H_
#define TEXT_FUNCTIONS_H_

#include "fonts.h"

struct display {
  unsigned int width;
  unsigned int height;
  unsigned int *buffer;
};

void print_char(char num, int x_offset, int y_offset, struct abc_font_struct font[], struct display disp);
//void print_string(char str[], int x_offset, int y_offset, struct abc_font_struct font[], struct display disp);
void print_string(char str[], int x_offset, int y_offset, struct display disp);
void print_support_text(struct display disp);

#endif /*TEXT_FUNCTIONS_H_*/
