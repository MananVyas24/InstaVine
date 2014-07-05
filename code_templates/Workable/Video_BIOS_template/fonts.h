#ifndef FONTS_H_
#define FONTS_H_


struct abc_font_struct {
  unsigned long extents_width;
  unsigned long extents_height;
  unsigned long extents_ascent;
  unsigned long extents_descent;
  unsigned long bounds_width;
  unsigned long bounds_height;
  unsigned char *char_alpha_map;
  unsigned long reserved;
};

extern struct abc_font_struct tahomabold_32[];
//extern struct abc_font_struct tahomabold_20[];

#endif /*FONTS_H_*/
