#ifndef GPROF_H
#define GPROF_H

/* Initialize profiler */
void gprof_init(void);

/* Add a program counter sample (called on timer interrupt) */
void gprof_add_sample(unsigned pc);

/* Print the profiler results */
void gprof_dump(void);


#endif
