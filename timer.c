#include "timer.h"

unsigned int timer_get_time(void) {
    return *(unsigned int volatile*)(0x20003000 + 0x4);
}


void delay_us(unsigned int n) {
    unsigned int start = timer_get_time();
    while (timer_get_time() - start < n) { /* spin */ }
}

void delaySec(unsigned int n){
    delay_us(n * 1000000);
}
