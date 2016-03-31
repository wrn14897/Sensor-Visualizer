/*#include <stdio.h>*/
#include "printf.h"
#include "gl.h"
#include "sine.h"

#define ALPHA    0xFF000000
#define RGBABITS 4 //bytes
#define DISTBETWEENCHARS 4 //pixels
#define ASCVALUPPERBOUND 127
#define ASCVALLOWERBOUND 32
#define NULL     0

//naive implementation of abs
/*int abs(int i){*/
  /*return (i < 0 ? -i : i);*/
/*}*/



void gl_init(unsigned width, unsigned height, unsigned mode){
  fb_init(width, height, RGBABITS, mode);
}
unsigned gl_get_width(void){ return fb_get_width();}
unsigned gl_get_height(void){ return fb_get_height();}

void gl_swap_buffer(void){
  fb_swap_buffer(); 
}

color gl_color(unsigned char r, unsigned char g, unsigned char b){
  return r  + (g << 8) + (b << 16) + ALPHA;
}

void gl_clear(color c){
  unsigned w = fb_get_width();
  unsigned h = fb_get_height();
  unsigned (*im)[w] = (unsigned (*)[w])fb_get_draw_buffer();
  for (int x = 0; x < w; x++){
    for (int y = 0; y < h; y++){
      im[y][x] = c;
    }
  }
}

void gl_draw_pixel(int x, int y, color c){
  unsigned w = fb_get_width();
  unsigned h = fb_get_height();
  //Error checking
  if (x < 0 || y < 0 || x >= w || y >= h) return;

  unsigned (*im)[w] = (unsigned (*)[w])fb_get_draw_buffer();
  im[y][x] = c;
}

void gl_draw_line( int x1, int y1, int x2, int y2, color c){
  int w = x2 - x1;
  int h = y2 - y1;
  int dx1 = 0, dy1 = 0, dx2 = 0, dy2 = 0;

  if (w<0) dx1 = -1; else if (w>0) dx1 = 1;
  if (h<0) dy1 = -1; else if (h>0) dy1 = 1;
  if (w<0) dx2 = -1; else if (w>0) dx2 = 1;
  
  int longest  = abs(w);
  int shortest = abs(h);
  if (!(longest>shortest)) {
    longest = abs(h);
    shortest = abs(w);
    if (h<0) dy2 = -1; else if (h>0) dy2 = 1;
    dx2 = 0;
  }
  int numerator = longest >> 1;
  for (int i=0;i<=longest;i++){
    gl_draw_pixel(x1, y1, c);
    numerator += shortest;
    if (!(numerator<longest)){
      numerator -= longest;
      x1 += dx1;
      y1 += dy1;
    }
    else{
      x1 += dx2;
      y1 += dy2;
    }
  }
}

void gl_draw_hline(int y, color c){
  if (y < 0 || y >= fb_get_height()) return;
  for( int x = 0; x < fb_get_width(); x++ )
    gl_draw_pixel(x, y, c);
}

void gl_draw_vline(int x, color c){
  if (x < 0 || x >= fb_get_width()) return;
  for( int y = 0; y < fb_get_height(); y++ )
    gl_draw_pixel(x, y, c);
}

void gl_draw_perspecGrid(int y, int dx, color c){
  int w = fb_get_width();
  gl_draw_line(0, y, w>>1, 0, c);

}

void gl_draw_filled_circle(int x, int y, int r, color c){
  int rr   = r * r;
  int rrrr = rr * rr;
  int x0   = r;
  int dx   = 0;

  for (int i = -r; i <= r; i++){
    gl_draw_pixel(x + i, y, c);
  }
  for (int dy = 1; dy <= r; dy++){
    int x1 = x0 - (dx - 1);
    for ( ; x1 > 0; x1--)  
      if (x1*x1*rr + dy*dy*rr <= rrrr)
        break;
    dx = x0 - x1;  // current approximation of the slope
    x0 = x1;
    for (int i = -x0; i <= x0; i++){
      gl_draw_pixel(x + i, y-dy, c);
      gl_draw_pixel(x + i, y+dy, c);
    }
  }
}

void gl_draw_empty_circle(int x, int y, int r, int start_degree, int end_degree, color c){
  if (end_degree < start_degree) return;
   
  int preX, preY, curX, curY;
  preX = x + (r*cos(0)/MAGFACTOR);
  preY = y + (r*sin(0)/MAGFACTOR);
  for (int theta = start_degree; theta < end_degree; theta++){
    curX = x + (r*cos(theta)/MAGFACTOR);
    curY = y + (r*sin(theta)/MAGFACTOR);
    gl_draw_line( preX, preY, curX, curY, c);
    preX = curX;
    preY = curY; 
  }
}

void gl_draw_rect(int x, int y, int w, int h, color c){
  if (x < 0 || y < 0 || w < 0 || h < 0) return;
  if (x + w >= fb_get_width() || y + h >= fb_get_height()) return;


  for( int i = x; i < x + w; i++){
    gl_draw_pixel(i, y, c);
    gl_draw_pixel(i, y + h, c);
  }
  for( int j = y; j < y + h; j++){
    gl_draw_pixel(x, j, c);
    gl_draw_pixel(x+w, j, c);
  }
  gl_draw_pixel(x+w, y+h, c);
}

void gl_draw_filled_rect(int x, int y, int w, int h, color c){
  if (x < 0 || y < 0 || w < 0 || h < 0) return;
  if (x + w >= fb_get_width() || y + h >= fb_get_height()) return;
  
  for (int i = x; i < x + w; i++){
    for (int j = y; j < y + h; j++){
      gl_draw_pixel(i, j, c);
    }
  }
}

void gl_draw_cursor(int x, int y, color c){
  int f_w = font_get_width();
  int f_h = font_get_height();
  for (int i = x; i < x + (f_w>>1); i++){
    for (int j = y; j < y+f_h; j++){
      gl_draw_pixel(i, j, c);
    }
  }
}


void gl_draw_char(int x, int y, char letter, color c){
  unsigned w = fb_get_width();
  unsigned h = fb_get_height();
  unsigned (*im)[w] = (unsigned (*)[w])fb_get_draw_buffer();

  int f_w = font_get_width();
  int f_h = font_get_height();
  int f_d = font_get_depth();

  //Error checking
  if ((letter < ASCVALLOWERBOUND) || (letter > ASCVALUPPERBOUND) || x + f_w >= w || y + f_h >= h) return;

  char fontMap[font_get_size()];

  if(!font_get_char(letter, fontMap, font_get_size())) return;

  int i, j, r;
  /*int comp = 0xffffff00;*/
  /*color currColor;*/
  for (i = 0; i < f_h; i++) {
    for (j = 0; j < f_w; j++) {
      r = fontMap[i * f_w * f_d + j * f_d];
      if ( (r & 0xff) >> 7 )
        im[y+i][x+j] = c;
    }
  }

}
void gl_draw_string( int x, int y, char* string, color c){
  unsigned w = fb_get_width();
  unsigned h = fb_get_height();
  int f_w = font_get_width();
  int f_h = font_get_height();
  char *ptr = string;

  while (*ptr != '\0'){
    if (*ptr == '\n'){
      y += f_h; 
      x = 0;
    }
    //Check boundaries
    if (x + f_w >= w){
      y += f_h; 
      x = 0;
    }
    if (y + f_h >= h) return;
    gl_draw_char(x, y, *ptr, c);  

    ptr ++;
    x += f_w; 
  }
}




