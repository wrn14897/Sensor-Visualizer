#ifndef CONSOLE_H
#define CONSOLE_H

#include "printf.h"
#include "malloc.h"
#include "gl.h"


unsigned getBufferIndexByXY(unsigned x, unsigned y);
void uploadWholeBufferToFB();
void writeCharToBuffer(int x, int y, char ch);
void scrollTextByOneLine();
void updateCursor(char *str);
void setCursorColor(color c);

void console_clear();

void console_init( unsigned nrows, unsigned ncols );

void console_putc( int ch );

void console_printf( const char *format, ... );

char console_get_char( const unsigned row, unsigned col );
int  console_get_nrows();
int  console_get_ncols();

char* getConsoleBuffer();

#endif
