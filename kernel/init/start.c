#include <xv6/types.h>
#include <xv6/param.h>
#include <arch/riscv/rv2/memlayout.h>
#include <arch/riscv/riscv.h>
#include <xv6/defs.h>

extern void _entry();
extern void main();
void timerinit();

// entry.S needs one stack per CPU.
__attribute__((aligned(16))) char stack0[4096 * NCPU];

// entry.S jumps here in supervisor mode on stack0.
void
start()
{
  w_satp(0); // paging is off temporarily
	     
  // enables S-mode to receive these interrupt types once they arrive
  // w_sie(r_sie() | SIE_SEIE | SIE_STIE);
  // timerinit();
	
  // OpenSBI only configures hart 0, so
  // we must start the remaining harts
  // via a SBI call.
  // if (cpuid() == 0) {
    // Start up remaining harts and make them jump to _sentry
  // }

  main();   // no mret - call main directly
}

void
timerinit()
{

  // ask for the very first timer interrupt, TIMER_TICKS from now
  w_stimecmp(r_time() + TIMER_TICKS);
}
