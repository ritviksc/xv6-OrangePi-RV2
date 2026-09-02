#include <xv6/types.h>
#include <xv6/param.h>
#include <arch/riscv/rv2/memlayout.h>
#include <arch/riscv/riscv.h>
#include <xv6/defs.h>
#include <sbi/sbi_legacy.h>

uint64 timer;

extern void kernelvec();
extern void main();
void timerinit();

// entry.S needs one stack per CPU.
__attribute__((aligned(16))) char stack0[4096 * NCPU];

// entry.S jumps here in supervisor mode on stack0.
void
start()
{
  w_satp(0); // paging is off temporarily

  // Enable interrupts in S-mode
  w_sstatus(r_sstatus() | SSTATUS_SIE);
  w_stvec((uint64)kernelvec);	   
  // enables S-mode to receive these interrupt types once they arrive
  w_sie(r_sie() | SIE_SEIE | SIE_STIE);
  
  timerinit();

  main();   // no mret - call main directly
}

void
timerinit()
{
  // ask for the very first timer interrupt, TIMER_TICKS from now
  timer += r_time() + TIMER_TICKS;
  // sbi_set_timer(timer);

  // The ky x1 supports sstc extension so we can configure timer directly
  // from S-mode
  w_stimecmp(timer);
  
}
