#include "timer-int.h"
#include "proto.h"
#include "gprof.h"

volatile int n_timer_interrupts;

#define RPI_INTERRUPT_CONTROLLER_BASE   0x2000B200
volatile rpi_irq_controller_t *rpi = (void*)RPI_INTERRUPT_CONTROLLER_BASE;

#define RPI_BASIC_ARM_TIMER_IRQ         (1 << 0)
#define RPI_ARMTIMER_BASE               0x2000B400
volatile arm_sys_timer_t *sys_timer = (void*)RPI_ARMTIMER_BASE;

/* Compile-time assertion used in function. */
#define AssertNow(x) switch(1) { case (x): case 0: ; }

void  setup_timer_init(unsigned ticks) { 
	n_timer_interrupts = 0;
	AssertNow(sizeof sys_timer->control == 4);

	sys_timer->control.enable_timer = 1;
	sys_timer->load = ticks;
	sys_timer->control.enable_interrupt = 1;
	sys_timer->control.prescale = 0b10;
    	rpi->Enable_Basic_IRQs = 1;

        system_enable_interrupts();
}

void timer_int_handler(unsigned int pc) {
  /* TODO: Fill this in */
  n_timer_interrupts ++;
  gprof_add_sample(pc);
}
