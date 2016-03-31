#include "visualizer.h"

#define TRUE  1
#define FALSE 0
#define VUMAX 10
#define VISUAL_MODE_VU  0
#define VISUAL_MODE_CIR 1

static int y_btn  = 0;
static int y_top  = 0;
static int y_VU   = 0;
static int sensitivity = 2;



void drawVUMeter(int x, int y, int val){
  int brick_w = font_get_width() << 3;
  int brick_h = font_get_height();
  for (int dy = 1; dy <= val; dy ++){
    if (dy <= 6)
      gl_draw_filled_rect(x, y - (dy*( (brick_h >> 1) + brick_h )), brick_w, brick_h, GL_GREEN);
    else if (6 < dy && dy <= 8)
      gl_draw_filled_rect(x, y - (dy*( (brick_h >> 1) + brick_h )), brick_w, brick_h, GL_YELLOW);
    else if (dy <= VUMAX) 
      gl_draw_filled_rect(x, y - (dy*( (brick_h >> 1) + brick_h )), brick_w, brick_h, GL_RED);
  }
}


//Here maximum value = 1024
void drawCircleMeter(int x0, int y0, int val){
  
  //Draw model
  gl_draw_empty_circle(x0, y0, MAGFACTOR, 1, 360, GL_TURQUOISE);
  gl_draw_empty_circle(x0, y0, 5, 1, 360, GL_TURQUOISE);
  gl_draw_line(x0, y0, x0 + cos(-225), y0 + sin(-225), GL_TURQUOISE); 
  gl_draw_line(x0, y0, x0 + cos(45),   y0 + sin(45), GL_TURQUOISE); 


  char valStr[5];
  my_snprintf(valStr, 5, "%d", val);
  int font_w = font_get_width();
  int valStrLen = getStrLen(valStr);
  gl_draw_string(x0 - font_w * (valStrLen>>1), y0 + (MAGFACTOR>>1), valStr, GL_PURPLE);
  



  int curX, curY, phi;
  phi =  -225 + (val>>2);
  if (phi > 45) phi = 45; //check boundary
  curX = x0 + cos(phi);
  curY = y0 + sin(phi);
  gl_draw_line(x0, y0, curX, curY, GL_TURQUOISE);
  

  int x1, y1, x2, y2;
  for (int theta = -224; theta < 45; theta++){
    //Draw marks 
    if (theta % 5 == 0){
      gl_draw_line(x0 + cos(theta), y0 + sin(theta),
                   x0 + (19*cos(theta))/20, y0 + (19*sin(theta))/20,
                   GL_YELLOW); 
    } 
    x2 = x0 + (cos(theta)>>1);
    y2 = y0 + (sin(theta)>>1);
    if (theta <= phi){
      x1 = x0 + (3*cos(theta))/4;
      y1 = y0 + (3*sin(theta))/4;
      gl_draw_line(x1, y1, x2, y2, GL_CRIMSON); 
    }
    else{
      x1 = x0 + (5*cos(theta))/8;
      y1 = y0 + (5*sin(theta))/8;
      gl_draw_line(x1, y1, x2, y2, GL_ROYALBLUE); 
    }
  }
}

void initVisualizerWindow(){
  int font_h = font_get_height();
  y_btn = 3*(gl_get_height()>>2);
  y_top = 1 + font_h;
  y_VU  = y_btn - font_h; 

  initGPIOForADC();
  //Write template to front and back buffers
  for (int i = 0; i < 2; i++){
    gl_clear(GL_BLACK);
    gl_draw_hline(0, GL_RED);
    gl_draw_string(0, 1, "Sensor Visualizer", GL_GREEN);
    gl_draw_hline(y_btn, GL_RED);
    gl_draw_string(0, y_btn+1, "Press 'q' to exit the program", GL_GREEN);
    gl_draw_string(0, y_btn+1+font_h, "Press 'a' to add or remove the visualizer", GL_GREEN);
    gl_draw_string(0, y_btn+1+2*font_h, "Press 's' to switch different visual modes", GL_GREEN);
    gl_draw_string(0, y_btn+1+3*font_h, "Press 'd' to display sensor measurements", GL_GREEN);
    /*gl_draw_string(0, y_btn+1+font_h, "Press 'a' or 'r' to add or remove the visulizer", GL_GREEN);*/
    gl_swap_buffer();
  }

  //Draw grid
  /*gl_draw_perspecGrid(y_btn-1, 5, GL_WHITE);*/
  /*gl_draw_circle(gl_get_width()>>1, y_btn-20, 20, GL_WHITE);*/
  /*gl_swap_buffer();*/
}
void clearVisualArea(){
  int w = gl_get_width();
  int font_h = font_get_height();
  unsigned (*im)[w] = (unsigned (*)[w])fb_get_draw_buffer();
  int y, x;
  for (y = y_top; y < y_btn; y++){
    for (x = 0; x < w; x++){
      im[y][x] = GL_BLACK;
    }
  }
}


void visualizerStart(int enableVisualizer, int mode, int displayVal){
  clearVisualArea();

  //Read data from MCP3008
  int adcCh1 = 0;
  int adcCh2 = 1;
  int font_h = font_get_height();
  /*int last_read = 0;*/
  int audioLevel = readadc(adcCh1);
  int lightLevel = readadc(adcCh2) * sensitivity;
  char adcReadStr[30];
  //Display basic interface
  if (displayVal){
    my_snprintf(adcReadStr, 30, "Channel 1 Level: %d\n", audioLevel);
    gl_draw_string(0, y_top, adcReadStr, GL_PURPLE);
    my_snprintf(adcReadStr, 30, "Channel 2 Level: %d\n", lightLevel);
    gl_draw_string(0, y_top + font_h, adcReadStr, GL_PURPLE);
    /*printf("%d\n", lightLevel);*/
  }

  //Display visualizer
  int glWidth = gl_get_width();
  if (enableVisualizer){
    if (mode == VISUAL_MODE_VU){
      //Audio level
      drawVUMeter(glWidth>>3, y_VU, (unsigned)diviu10(diviu10((unsigned)audioLevel)));
      //Light level -> (val / 1000) ~= (val / 1024)
      drawVUMeter(5*(glWidth>>3), y_VU, (unsigned)diviu10(diviu10((unsigned)lightLevel)));
    }
    else{
      drawCircleMeter(glWidth>>2,     y_VU - (y_btn - y_top)/2, audioLevel);
      drawCircleMeter(3*(glWidth>>2), y_VU - (y_btn - y_top)/2, lightLevel);
    }

  }
  gl_swap_buffer();  

}

