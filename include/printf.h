#ifndef PRINTF_H_INCLUDED
#define PRINTF_H_INCLUDED

#include <stdarg.h>
//Macros here
#define FALSE 0
#define TRUE  1
#define MAXOPTIONS      128
#define MAXINTDIGITS    10
#define MAXWIDTH        32
#define MAXSTRINGLENGTH 1000
#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })
#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })


unsigned diviu10(unsigned n);
void shiftCharsToFront(char *buf, int n, int start_i);
void strCopy(char *buf, const char *target);
int powOfNum(int num, int n);
int getStrLen(const char *str);
char* inttobin( char *buf, int n, unsigned val, int width);
char* inttodec( char *buf, int n, int val, int width);
char* inttohex( char *buf, int n, unsigned val, int width);
int myAtoi(char* chs);
int getOptions(char* input, char* options, int* widths);
void replaceWithCorrChars(char *buf, int n, char *format, char bufs[MAXOPTIONS][MAXWIDTH]);
int my_snprintf(char *buf, int n, char *format, ...);
int my_vsprintf(char *buf, char *format, va_list arg);

/*
 * The following functions need to be implemented on PI
 */
#ifndef LOCAL_TEST

#include "gpio.h"
#include "uart.h"

void putStr(char *buf);
int printf(char *format, ...);

#define assert(EXP)\
do{if(0==(EXP)){\
    printf("Assertion failed: %s, %s(), %d\n",\
    __FILE__,\
    __func__,\
    __LINE__);\
    gpio_set_function(GPIO_PIN47, GPIO_FUNC_OUTPUT);\
    gpio_write(GPIO_PIN47, 1);\
    }\
}while(0)
#endif


#endif
