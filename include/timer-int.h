/* timer.c */
void setup_timer_init(unsigned ticks);
void timer_int_handler(unsigned int pc);
extern volatile int n_timer_interrupts;

// http://xinu.mscs.mu.edu/BCM2835_Interrupt_Controller
// BCM: p196
typedef volatile struct {
    unsigned IRQ_basic_pending;
    unsigned IRQ_pending_1;
    unsigned IRQ_pending_2;
    unsigned FIQ_control;
    unsigned Enable_IRQs_1;
    unsigned Enable_IRQs_2;
    unsigned Enable_Basic_IRQs;
    unsigned Disable_IRQs_1;
    unsigned Disable_IRQs_2;
    unsigned Disable_Basic_IRQs;
} rpi_irq_controller_t;

// p 196
typedef volatile struct {
        unsigned load;
        unsigned value;                 // read only
        struct {
                unsigned :1,
                         counter_is_23_bit:1,   // 1 = 23 bit, 0 = 16
                         prescale:2,            // 00 = clock / 1
                                                // 01 = clock/16
                                                // 10 = clock / 256
                                                // 11 = clock / 1 [undef?]
                        :1,
                        enable_interrupt:1,             // 1 = timer int enabled
                        :1,
                        enable_timer:1,
                        run_in_debug:1,         // 1 = run timer in debug
                        disable_free_timer:1,
                        :6,
                        free_counter_scale:8;     // clk/(prescale+1)
        } control;
        unsigned irq_clear;             // write only
        unsigned raw_irq;               // read only
        unsigned masked_irq;            // read only
        unsigned reload;
        unsigned pre_div;               // not in real 804. [means?]
        unsigned free_counter;          // free running counter not in 804?
} arm_sys_timer_t;

