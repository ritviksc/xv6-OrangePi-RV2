#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"

extern void _entry();
extern void sbi_hart_start(uint64 hartid, uint64 start_addr, uint64 opaque);
extern void main();
void timerinit();

// entry.S needs one stack per CPU.
__attribute__((aligned(16))) char stack0[4096 * NCPU];

// if any harts fail to start up - record them
bool hart_start_failed[NCPU];

// entry.S jumps here in supervisor mode on stack0.
void
start()
{
  w_satp(0); // paging is off temporarily
	     
  // enables S-mode to receive these interrupt types once they arrive
  w_sie(r_sie() | SIE_SEIE | SIE_STIE);
  timerinit();
	
  // OpenSBI only configures hart 0, so
  // we must start the remaining harts
  // via a SBI call
  if (cpuid() == 0) {
    for (uint64 i = 1; i < NCPU; i++){
      long err = sbi_hart_start(i,(uint64)_entry,0);
      if (err != 0) {
        hart_start_failed[i] = true;    
      }

    }
  }

  main();   // no mret -- call main directly
}

void
timerinit()
{

  // ask for the very first timer interrupt, TIMER_TICKS from now
  w_stimecmp(r_time() + TIMER_TICKS);
}
