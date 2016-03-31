#include "proto.h"
#include "gprof.h"
#include "gpio.h"
#include "timer-int.h"
#include "circular.h"
#include "keyboard.h"
#include "console.h"
#include "commands.h"
#include "strtol.h"
#include "reboot.h"
#include "timer.h"
#include "malloc.h"
#include "visualizer.h"

#define NROWS 30
#define NCOLS 50

// This function should not be called.
void impossible_vector(unsigned pc) {
  printf("impossible exception at pc=%x\n", pc);
  reboot();
}

void interrupt_vector(unsigned pc) {
  // FIXME: Handle interrupts.
  keyboard_int_handler(pc);
}

#define MAXCOMMANDSTRSLENGTH 50
#define MAXCOMMANDWORDSNUM   20
#define COMMANDSNUM           9
#define KEYCODE_ESC          32

static int ISCOMMANDMODE = FALSE;

//Declare some helper functions here
int extractWordsFromCmd(char *cmd, char *commandWords[MAXCOMMANDSTRSLENGTH]);
void toLower(char *str);
int strcmp(char *str1, char *str2);



static const command_t commands[] = {
  {"help",   "[cmd] prints a list of commands or description of cmd", cmd_help},
  {"echo",   "<...> echos the user input to the screen", cmd_echo},
  {"clear",  "clear the screen", cmd_clear},
  {"reboot", "reboot the pi", cmd_reboot},
  {"peek",   "[address][nbytes] get the number stored in the address given n bytes", cmd_peek},
  {"poke",   "[address][value] save the number to the given address", cmd_poke},
  {"quit",   "exit debugging mode", cmd_quit},
  {"profile","[on | off | status | results] track timer interrupts", cmd_profile},
  {"vs",     "run sensor visualizer program", cmd_visualizer},
};

/**
 * Custom functions
 */
void testReadADC(){
  initGPIOForADC();
  int adcNum = 0;
  int last_read = 0;
  int adcRead = 0;
  while(1){
    adcRead = readadc(adcNum);
    printf("%d\n", adcRead);
      
    /*if (last_read != adcRead){*/
      /*printf("Bang !!\n");*/
    /*}*/
    /*last_read = adcRead;*/
    /*if (adcRead < 30)*/
      /*printf("Bang !!\n");*/
  }
}


int cmd_visualizer(int argc, const char *argv[]){
   
  //Init program windwo
  initVisualizerWindow();

  //Main visualizer program here....
  int enableVisualizer = FALSE;
  int displayVal       = FALSE;
  int visualMode       = 0;
  while (1){  
    char c = keyboard_read();
    if (c == 'a') enableVisualizer = (~enableVisualizer);
    if (c == 'd') displayVal = (~displayVal);
    if (c == 's') visualMode = (~visualMode);
    if (c == 'q') break;
    visualizerStart(enableVisualizer, visualMode, displayVal);
  }
  
  console_putc('\n');  
  console_putc('\b');  
  return 0;
}


/**
 * Basic commands 
 */
int cmd_reboot(int argc, const char *argv[]){
  reboot();
  return 0;
}

int cmd_clear(int argc, const char *argv[]){
  console_clear();
  return 0;
}

int cmd_echo(int argc, const char *argv[]) {
  for (int i = 1; i < argc; ++i) {
    console_printf(argv[i]);
    console_printf(" ");
  }
  console_printf("\n");
  return 0;
}


int cmd_help(int argc, const char *argv[]) {
  // FIXME: Part 7.
  //
  //Print out all commands' helps
  if (argc == 1){
    for (int i = 0; i < COMMANDSNUM; i++){
      console_printf("%s - %s\n", commands[i].name, commands[i].description);
    }
  }
  else{
    char *targetCmd = (char *)argv[1];
    for (int i = 0; i < COMMANDSNUM; i++){
      if (strcmp(targetCmd, commands[i].name)){
        console_printf("%s - %s\n", commands[i].name, commands[i].description);
        return 0;
      }
    }
    console_printf("error: '%s' is not a valid command\n", targetCmd);
  }
  return 0;
}

int cmd_peek(int argc, const char *argv[]){
  char *addressCheck;
  char *nbytesCheck;

  long address = strtol(argv[1], &addressCheck, 0);
  long nbytes  = (argc == 2) ? 1 : strtol(argv[2], &nbytesCheck, 0);
  if (addressCheck == (char *)argv[1] || *addressCheck != '\0' || *nbytesCheck != '\0' || nbytes > 4){
    console_printf("error: input args(address or nbytes) are not valid\n");
  }
  else{
    /**(unsigned int*)address = 0x12345678;*/
    char* p = (char *)address; 
    //Move up by 3 bytes 
    p -= 3;

    for (int i = 0; i < nbytes; i++){
      console_printf("%02x ", *p++);
    }
    console_putc('\n');
  }
  return 0;
}

int cmd_poke(int argc, const char *argv[]){
  if (argc < 3){
    console_printf("error: input args not enough [address][value]\n");
    return 0;
  }
  char *addressCheck = 0;
  char *valueCheck  = 0;
  long address = strtol(argv[1], &addressCheck, 0);
  long value   = strtol(argv[2], &valueCheck, 0);
  if (addressCheck == (char *)argv[1] || valueCheck == (char *)argv[2] || *addressCheck != '\0' || *valueCheck != '\0'){
    console_printf("error: input args(address or value) are not valid\n");
  }
  else {
    *(long *)address = value; 
    console_putc('\n');
  }
  return 0;
}

int cmd_quit(int argc, const char *argv[]){
  ISCOMMANDMODE = FALSE;
  setCursorColor(GL_GREEN);
  return 0;
}

int cmd_profile(int argc, const char *argv[]){
  if (argc < 2){
    console_printf("error: input args not enough [on | off | status | results]\n");
    return 0;
  }
  char *targetCmd = (char *)argv[1];
  toLower(targetCmd);

  if (strcmp(targetCmd, "on")){
    gprof_init();  
  }
  else if (strcmp(targetCmd, "off")){

  }
  else if (strcmp(targetCmd, "status")){

  }
  else if (strcmp(targetCmd, "results")){
    gprof_dump();
  }
  else{
    console_printf("error: input args not valid [on | off | status | results]\n");
  }

  return 0;
}





//Function assumes words were seperated by spaces
int extractWordsFromCmd(char *cmd, char *commandWords[MAXCOMMANDSTRSLENGTH]){
  char *cmdPtr = cmd;
  int nwords = 0;
  while(1){
    while(*cmdPtr == ' ' || *cmdPtr == '\n')
		  cmdPtr++;

	  if(*cmdPtr == '\0')
		  return nwords;

	  commandWords[nwords++] = cmdPtr;

	  while((*cmdPtr != ' ') && (*cmdPtr != '\0'))
		  cmdPtr++;

	  if(*cmdPtr == '\0')
		  return nwords;

	  *cmdPtr++ = '\0';

	  if(nwords >= MAXCOMMANDWORDSNUM)
		  return nwords;
	}
  /*DEBUGGING*/
  /*for (int i = 0; i < nwords; i++)*/
    /*printf("%s\n", commandWords[i]);*/
}
void toLower(char *str){
  for(char *p = str;*p;++p) *p=*p>0x40&&*p<0x5b?*p|0x60:*p;
}


//Compare two words
int strcmp(char *str1, char *str2){
  int ctr=0;
  
  while(str1[ctr]==str2[ctr]){
    if(str1[ctr]=='\0'||str2[ctr]=='\0')
      break;
    ctr++;
  }
  if(str1[ctr]=='\0' && str2[ctr]=='\0')
    return TRUE;
  else
    return FALSE;
}







void main(void) {
  uart_init();
  keyboard_init();
  console_init(NROWS, NCOLS);
  
  /*testReadADC();*/
  /*cmd_visualSnd(0, "");*/
  char commandStr[MAXCOMMANDSTRSLENGTH];
  char *commandWords[MAXCOMMANDSTRSLENGTH];
  
  char *cmdPtr = commandStr;

  while (1) {
    char c = keyboard_read();
  	if (c == KEYCODE_ESC){
      setCursorColor(GL_BLUE);
      ISCOMMANDMODE = TRUE;
      console_putc('\n');
    }

    //Command mode
    while (ISCOMMANDMODE){
    	c = keyboard_read();
    	if (c > 0){
		    console_putc(c);
		    /*Enter commands...*/
		    if ( (cmdPtr - commandStr >= MAXCOMMANDSTRSLENGTH) || (c == '\n')){
		            
		      int nwords = extractWordsFromCmd(commandStr, commandWords);
		      //Process commands
		      char *cmd = commandWords[0];
		      toLower(cmd);
		      for (int i = 0; i < COMMANDSNUM; i++){
		        if (strcmp(cmd, commands[i].name)){
		          commands[i].fn(nwords, commandWords);
		        }
		      }
		      
		    
		      /*Clear cmd buffer*/
		      for (int i = 0; i < (cmdPtr - commandStr); i++){
		        commandStr[i] = '\0';
		      }
		      cmdPtr = commandStr; 
		    }
		    //Add or delete char
		    if (c == '\b' && cmdPtr > commandStr){
		      cmdPtr --;
		      *cmdPtr = '\0';
		    }
		    else{
		      *cmdPtr = c;
		      cmdPtr ++;
	      }
		  }
	  }
    if (c > 0){
      console_putc(c);
    }
  }
  reboot();
}
