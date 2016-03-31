#include "proto.h"
#include "timer-int.h"
#include "malloc.h"
#include "console.h"

static unsigned *gprof_counts,
                gprof_size;

volatile unsigned gprof_samples, gprof_overrun;

static const unsigned text_base = 0x8000;

static unsigned pc_to_index(unsigned pc) { return (pc - text_base) / 4; }
static unsigned index_to_pc(unsigned idx) { return idx * 4 + text_base; }


void gprof_init(void) { 
    extern char __text_end__;
    gprof_size = ((char *)&__text_end__ - (char*)text_base) / 4;
    gprof_counts = malloc(gprof_size * sizeof *gprof_counts);
    int i;
    printf("allocating %d entries\n", gprof_size);
    for(i = 0; i < gprof_size; i++)
        gprof_counts[i] = 0;
    gprof_samples = 0;
}

void gprof_dump(void) {
    int i;
    console_printf("*****************************************************\n");
    console_printf(" PC    |     COUNT\n");
    for(i = 0; i < gprof_size; i++) {
        unsigned cnt = gprof_counts[i];
        if(cnt)
            console_printf("%x:\t%d count\n", index_to_pc(i), cnt);
    }
}

void gprof_add_sample(unsigned pc) {
    unsigned index = pc_to_index(pc);
    if(index >= gprof_size) {
        /* should not happen unless dynamically generate code */
        gprof_overrun++;
    } else {
        gprof_counts[index]++;
        gprof_samples ++;
    }
}
