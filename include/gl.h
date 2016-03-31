#ifndef GL_H
#define GL_H

#include "fb.h"
#include "font.h"

#define GL_SINGLEBUFFER FB_SINGLEBUFFER
#define GL_DOUBLEBUFFER FB_DOUBLEBUFFER

void gl_init(unsigned width, unsigned height, unsigned mode);

#define GL_BLACK       0xFF000000
#define GL_WHITE       0xFFFFFFFF
#define GL_RED         0xFF0000FF
#define GL_GREEN       0xFF00FF00
#define GL_BLUE        0xFFFF0000
#define GL_YELLOW      0xFF00FFFF
#define GL_DODGERBLUE  0xFF8B4E10  
#define GL_DEEPSKYBLUE 0xFFFFBF00
#define GL_CRIMSON     0xFF3C14DC
#define GL_ROYALBLUE   0xFFCD5F3A
#define GL_TURQUOISE   0xFFFFF500
#define GL_ORANGE      0xFF007FFF
#define GL_PURPLE      0xFF9B30FF

typedef unsigned color;

void gl_swap_buffer(void);

color gl_color( unsigned char r, unsigned char g, unsigned char b );

unsigned gl_get_width(void);
unsigned gl_get_height(void);

void gl_clear(color c);

void gl_draw_pixel(int x, int y, color c);

void gl_draw_hline(int y, color c);

void gl_draw_vline(int x, color c);

void gl_draw_rect(int x, int y, int w, int h, color c);

void gl_draw_filled_rect(int x, int y, int w, int h, color c);

void gl_draw_cursor(int x, int y, color c);

void gl_draw_char(int x, int y, char letter, color c);

void gl_draw_string( int x, int y, char* string, color c);

void gl_draw_perspecGrid(int y, int dx, color c);

void gl_draw_filled_circle(int x, int y, int r, color c);

void gl_draw_empty_circle(int x, int y, int r, int start_degree, int end_degree, color c);

unsigned gl_get_char_height(void);
unsigned gl_get_char_width(void);

// The next two may be implemented for the extension
void gl_draw_line( int x1, int y1, int x2, int y2, color c);
void gl_draw_triangle( int x1, int y1, int x2, int y2, int x3, int y3, color c);

#endif
