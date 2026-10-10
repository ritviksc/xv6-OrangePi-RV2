#include <xv6/types.h>
#include <xv6/param.h>
#include <arch/riscv/rv2/memlayout.h>
#include <arch/riscv/riscv.h>
#include <xv6/defs.h>
#include <sbi/sbi_legacy.h>
#include <drivers/uart/sbi_uart.h>

uint64 timer;

extern void kernelvec();
extern void main();
void timerinit();


// entry.S needs one stack per CPU.
__aligned(16) char stack0[4096];

// entry.S jumps here in supervisor mode on stack0.
__noreturn void
start()
{
  w_sstatus(r_sstatus() & ~SSTATUS_SIE);
  w_sie(0);

  w_satp(0);
  __asm__ __volatile__ ("sfence.vma" ::: "memory");

  w_sie(SIE_SEIE | SIE_STIE);
  w_sstatus(r_sstatus() | SSTATUS_SIE);

  main();

  __unreachable;
  
}

void
timerinit()
{
  timer = r_time();
  timer +=  TIMER_TICKS;
  w_stimecmp(timer);
}
