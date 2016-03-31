/*#include <stdio.h>*/
#include <gpio.h>
#include <uart.h>
#include <proto.h>
#include "timer-int.h"
#include "circular.h"

#include "ps2.h"
#include "keyboard.h"

#define TRUE  1
#define FALSE 0
#define PS2CODEBITS 11

/* trivial little program to catch gpio interrupts on button pressing. */
#define RPI_INT_ENABLE_1             0x2000B210
#define RPI_INT_ENABLE_2             0x2000B214
#define RPI_INT_DISABLE_1            0x2000B21C
#define RPI_INT_DISABLE_2            0x2000B220


const unsigned CLK  = GPIO_PIN23;
const unsigned DATA = GPIO_PIN24;
static unsigned isSHIFTBeingPressed = FALSE;

void wait_for_falling_edge(unsigned pin) {
  while (gpio_read(pin) == 0) {  }
  while (gpio_read(pin) == 1) {  }
}

volatile unsigned int PS2Code[PS2CODEBITS];
volatile int cnt = 0;
volatile int currScancode = 0;
volatile int numOfOne = 0;
void keyboard_int_handler(unsigned pc){

  if (gpio_check_and_clear_event(CLK)){
    int bit = gpio_read(DATA);
    //Check start, parity and stop bit
    if ( (cnt == 0 && bit == 1) ||
         (cnt == 9 && (numOfOne & 1) && bit) ||
         (cnt == 9 && (!(numOfOne & 1) && !bit)) ||
         (cnt == 10 && bit == 0) ){
      cir_push(0);
      currScancode = 0;
      cnt = 0;
      return;
    }

    if (cnt > 0 && cnt < 9){
      if (bit) numOfOne ++;
      currScancode |= (bit << (cnt-1));
    }
    PS2Code[cnt++] = bit;

    //Package received
    if (cnt == 11){
      cir_push(currScancode);              
      /*printf("%08x\n", currScancode); */
      cnt = 0;
      currScancode = 0;
    }
  }
  else{
    timer_int_handler(pc);
  }
}


void keyboard_init(void) {
  gpio_init();
  // Ensure all interrupts are disabled.
  PUT32(RPI_INT_DISABLE_1, 0xFFFFFFFF);
  PUT32(RPI_INT_DISABLE_2, 0xFFFFFFFF);

  gpio_set_function(CLK, GPIO_FUNC_INPUT);
  gpio_set_pullup(CLK);
  gpio_detect_falling_edge(CLK);

  gpio_set_function(DATA, GPIO_FUNC_INPUT);
  gpio_set_pullup(DATA);

  // Enable GPIO interrupt 0
  PUT32(RPI_INT_ENABLE_2, 0x00100000);

  // Enable interrupts
  system_enable_interrupts(); 
}

/**
 * Reads a byte from the PS2 keyboard.
 *
 * @return the byte (a positive number) if the read was successful
 *         a negative number if the read was unsuccessful
 */
int keyboard_read_byte(void) {
  /*int bit = 0;*/
  /*int scancode = 0;*/
  /*unsigned dataBitNum = 0;*/

  //Check if key was pressed
  
  /*wait_for_falling_edge(CLK);*/
  /*bit = gpio_read(DATA);*/
  /*if(bit) return -1;*/
  
  /*for (int i = 0; i < 8; i++) {*/
    /*wait_for_falling_edge(CLK);*/
    /*bit = gpio_read(DATA);*/
    /*if (bit) dataBitNum ++;*/
    /*scancode |= (bit << i);*/
  /*}*/

  //Check parity
  /*wait_for_falling_edge(CLK);*/
  /*bit = gpio_read(DATA);*/
  /*printf("Bit num: %d, parity: %2x\n", dataBitNum, bit); */
  /*if ((dataBitNum & 1) && bit)   return -1; */
  /*if (!(dataBitNum & 1) && !bit) return -1;*/

  //Check stop bit
  /*wait_for_falling_edge(CLK);*/
  /*bit = gpio_read(DATA);*/
  /*if (!bit) return -1;*/
 
  /*printf("%08b\n", scancode); */

  int scancode = cir_pop();
  /*printf("%08b\n", scancode); */
  

  //Ignore specific codes
  if (scancode == KEYBOARD_CODE_RELEASE || scancode == KEYBOARD_CODE_VAR_IGNORE){ 
    if(keyboard_read_byte() == KEYBOARD_CODE_CAPS_LOCK)
      isSHIFTBeingPressed ^= 1;
    return -1;
  }
  //Check CAPSLOCK and SHIFT
  if (scancode == KEYBOARD_CODE_SHIFTL || scancode == KEYBOARD_CODE_SHIFTR || scancode == KEYBOARD_CODE_CAPS_LOCK){
    isSHIFTBeingPressed ^= 1;
  }

  return scancode;
}

/**
 * Reads a character from the PS2 keyboard. This happens in two steps. First,
 * the scancodes are read directly via the PS/2 protocol. Then, the scancodes
 * are converted to their ASCII character.
 *
 * @return the char (a positive number) if the read was successful
 *         0 if the read was unsuccessful
 */
char keyboard_read(void) {
  int scancode = keyboard_read_byte();
  if (scancode > 0) {
    /*printf("%8x -> %c\n", scancode, char_from_scan[scancode]);*/
    if (isSHIFTBeingPressed){
      return shift_char_from_scan[scancode];
    }
    return char_from_scan[scancode];
  } else {
    return 0;
  }
}
