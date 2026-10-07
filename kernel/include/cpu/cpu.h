#ifndef CPU_H
#define CPU_H

#include <proc/proc.h>

// Per-CPU state.
struct cpu {
  struct proc *proc;      // The process running on this cpu, or null.
  struct context context; // swtch() here to enter scheduler().
  int noff;               // Depth of push_off() nesting.
  int intena;             // Were interrupts enabled before push_off()?
};

extern struct cpu cpus[NCPU];


int		cpuid();
struct cpu *	mycpu(void);

#endif
