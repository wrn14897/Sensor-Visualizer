#include "console.h"

#define DISTBETWEENCAHARS 4
#define TEXTCOLOR   GL_GREEN
#define windowWidth  640
#define windowHeight 480

static char*    buf          = 0;
static int      cursorPosiX  = 0;
static int      cursorPosiY  = 0;
static unsigned wordsPerLine = 0;
static unsigned numOfLines   = 0;
static unsigned fontWidth    = 0;
static unsigned fontHeight   = 0;
static color    cursorColor  = GL_GREEN;

char* getConsoleBuffer(){
  return buf;
}

void setCursorColor(color c){
  cursorColor = c;
}

unsigned getBufferIndexByXY(unsigned x, unsigned y){
  return y*wordsPerLine + x;
}

void uploadWholeBufferToFB(){
  int x = 0;
  int y = 0;
  char *ptr = buf;
  while (*ptr != '\0'){
    //Check boundaries
    if (x  >= wordsPerLine){
      y += 1; 
      x = 0;
    }

    gl_draw_char(x*fontWidth, y*fontHeight, *ptr, TEXTCOLOR);  
    x ++; 
    ptr ++;
  }
}

void writeCharToBuffer(int x, int y, char ch){
  if (!ch) return;

  if (x == wordsPerLine-1 && y == numOfLines - 1){
    buf[getBufferIndexByXY(x, y)] = ch;
    scrollTextByOneLine();
    return;
  }
  if (ch == '\f'){
    char *ptr = buf;
    while (*ptr != '\0'){
      *ptr = '\0';
      ptr ++;
    }
    return;
  }
  if (ch == '\n'){
    buf[getBufferIndexByXY(x, y)] = ch;
    x ++;
    while(x < wordsPerLine){
      buf[getBufferIndexByXY(x, y)] = '\377';
      x ++;
    }
    y += 1;
    if (y >= numOfLines){
      scrollTextByOneLine();
    }
    return;
  }
  if (ch == '\b'){
    x -= 1;
    if (x < 0){
        y -= 1;
        x = wordsPerLine - 1;
        if (y < 0){
          x = 0;
          y = 0;
        }
        while(buf[getBufferIndexByXY(x, y)] == '\377'){
          x -= 1;
        }
    }
    ch = '\0';
  }

  buf[getBufferIndexByXY(x, y)] = ch;
}

void scrollTextByOneLine(){
  for (int y = 0; y < numOfLines-1; y++){
    for (int x = 0; x < wordsPerLine; x++){
      buf[getBufferIndexByXY(x, y)] = buf[getBufferIndexByXY(x,y+1)];
    }
  }
  //Clear the last line
  for (int x = 0; x < wordsPerLine; x++){
    buf[getBufferIndexByXY(x, numOfLines-1)] = '\0';
  }
}

void updateCursor(char *str){
  char *ptr = str;
  while (*ptr != '\0'){ 
    if (*ptr == '\f'){
      cursorPosiX = 0;
      cursorPosiY = 0;
    }
    else if (*ptr == '\n'){
      cursorPosiY += 1;
      cursorPosiX = 0;
    }
    else if (*ptr == '\b'){
      cursorPosiX -= 1;
      if (cursorPosiX < 0){
        cursorPosiY -= 1;
        cursorPosiX = wordsPerLine-1; 
        if (cursorPosiY < 0){
          cursorPosiY = 0;
          cursorPosiX = 0;
        }
        while (buf[getBufferIndexByXY(cursorPosiX, cursorPosiY)] == '\377'){
          cursorPosiX --;
        }
      }
    }
    else{
      cursorPosiX += 1;
    }

    //Check X
    if (cursorPosiX >= wordsPerLine){
      cursorPosiY += 1;
      cursorPosiX = 0;
    }
    //Check Y
    if (cursorPosiY >= numOfLines){
      cursorPosiY -= 1;
    }
    ptr ++;
  }
}
void console_clear(){
  gl_clear(GL_BLACK);
  cursorPosiX = 0;
  cursorPosiY = 0;
  for (int i = 0; i < (numOfLines*wordsPerLine); i++){
    buf[i] = '\0';
  }
  gl_draw_cursor(cursorPosiX, cursorPosiY, cursorColor);
  gl_swap_buffer();
}

void console_init( unsigned nrows, unsigned ncols ){
  //Init stuff
  wordsPerLine = ncols;
  numOfLines   = nrows;
  fontWidth    = font_get_width();
  fontHeight   = font_get_height();
  /*windowWidth  = wordsPerLine * fontWidth;*/
  /*windowHeight = numOfLines   * fontHeight;*/

  gl_init(windowWidth, windowHeight, GL_DOUBLEBUFFER);

  //Init buffer
  buf = malloc(sizeof(char) * numOfLines * wordsPerLine);
  console_clear();

}
void console_putc( int ch ){
  gl_clear(GL_BLACK);

  writeCharToBuffer(cursorPosiX, cursorPosiY, ch);
  uploadWholeBufferToFB();
  char tmp[2];
  tmp[0] = ch; 
  tmp[1] = '\0';
  updateCursor(tmp);
  gl_draw_cursor(cursorPosiX*fontWidth, cursorPosiY*fontHeight, cursorColor);

  gl_swap_buffer();
}
void console_printf( const char *format, ... ){
  //Initial error checking
  //if ((int)sizeof(buf) != n) return -1;
  if (format == 0) return; 


  //Define empty array at the beginning
  char buf[MAXSTRINGLENGTH];
  //Clear buf
  for(int i = 0; i < MAXSTRINGLENGTH; i++){
      buf[i] = '\0';
  }
  

  //Length checking
  if (getStrLen(format) > MAXSTRINGLENGTH - 1) return;

 

  //Array to store options
  char options[MAXOPTIONS];
  int  widths[MAXOPTIONS];
  int numOfSigns = getOptions(format, options, widths);
  

  if (numOfSigns < 0) return;
  //If no args
  if (numOfSigns == 0){
      int i = 0;
      while (format[i] != '\0' && i < (MAXSTRINGLENGTH-1)){
          buf[i] = format[i];
          i++;
      }
      buf[i] = '\0';
  }
  else{
    //Strings need to plug into format later
    char bufs[MAXOPTIONS][MAXWIDTH];
    //Consider args
    va_list ap;

    //Check each args
    va_start(ap, format);
        for (int i = 0; i < numOfSigns; i++){
            //Three cases => char, string, integer(signed, unsigned)
            switch(options[i]){
                case 'c':
                    bufs[i][0] = (char)va_arg(ap, int);
                    bufs[i][1] = '\0';
                    break;
                case 's':
                    strCopy(bufs[i], va_arg(ap, char*));
                    break;
                case 'd':
                    inttodec(bufs[i], MAXWIDTH, va_arg(ap, int), widths[i]);
                    break;
                case 'b':
                    inttobin(bufs[i], MAXWIDTH, va_arg(ap, unsigned), widths[i]);
                    break;
                case 'x':
                    inttohex(bufs[i], MAXWIDTH, va_arg(ap, unsigned), widths[i]);
                    break;
                default:
                    break;
            }
        }
    va_end(ap);
    /*
     * Here we are going to plug strings into format
     */
    replaceWithCorrChars(buf, MAXSTRINGLENGTH, format, bufs);
  }

  
  //Write a whole buffer immediately instead of calling console_putc each char
  gl_clear(GL_BLACK);
  char *ptr = buf;
  char tmp[2];
  while (*ptr){
    //Remove current cursor
    gl_draw_cursor(cursorPosiX*fontWidth, cursorPosiY*fontHeight, GL_BLACK);
    writeCharToBuffer(cursorPosiX, cursorPosiY, *ptr);
    tmp[0] = *ptr; 
    tmp[1] = '\0';
    updateCursor(tmp);
    ptr++;
  }

  //We need to write both front and back buffers 
  uploadWholeBufferToFB();
  gl_draw_cursor(cursorPosiX*fontWidth, cursorPosiY*fontHeight, cursorColor);

  gl_swap_buffer();

  uploadWholeBufferToFB();
  gl_draw_cursor(cursorPosiX*fontWidth, cursorPosiY*fontHeight, cursorColor);
}
char console_get_char( const unsigned row, unsigned col ){
  return buf[getBufferIndexByXY(col, row)];
}

int console_get_nrows(){
  return numOfLines;  
}
int console_get_ncols(){
  return wordsPerLine;
}
