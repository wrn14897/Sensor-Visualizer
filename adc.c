#include "printf.h"
#include "adc.h"

#define SPICLK   GPIO_PIN11
#define SPIMISO  GPIO_PIN9
#define SPIMOSI  GPIO_PIN10
#define SPICS    GPIO_PIN8
#define MAXVALUE 1024
#define FALSE   0
#define TRUE    1


void initGPIOForADC(void){
  //GPIO init
  gpio_init();
  gpio_set_function(SPICS,    GPIO_FUNC_OUTPUT);
  gpio_set_function(SPICLK,   GPIO_FUNC_OUTPUT);
  gpio_set_function(SPIMOSI,  GPIO_FUNC_OUTPUT);
  gpio_set_function(SPIMISO,  GPIO_FUNC_INPUT);
}

int readadc(int adcnum){
  if ((adcnum > 7) || (adcnum < 0)) return -1;
  
  /*gpio_write(SPICS,   TRUE);*/
  gpio_write(SPICLK,  FALSE);
  gpio_write(SPICS,   FALSE);

  int commandout = adcnum; 
  commandout |= 0x18;  // start bit + single-ended bit
  commandout <<= 3;    // we only need to send 5 bits here
  for (int i = 0; i < 5; i++){
    if (commandout & 0x80){
      gpio_write(SPIMOSI, TRUE);
    }
    else{
      gpio_write(SPIMOSI, FALSE);
    }
    commandout <<= 1;
    gpio_write(SPICLK, TRUE);
    gpio_write(SPICLK, FALSE);
  }  
  int adcout = 0;
  // read in one empty bit, one null bit and 10 ADC bits
  for (int i = 0; i < 12; i++){
    gpio_write(SPICLK, TRUE);
    gpio_write(SPICLK, FALSE);
    adcout <<= 1;
    if (gpio_read(SPIMISO)){
      adcout |= 0x1;
    }
  }
  gpio_write(SPICS, TRUE);
  adcout >>= 1;
  return adcout;
}




