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
__attribute__((aligned(16))) char stack0[4096];

// entry.S jumps here in supervisor mode on stack0.
void
start()
{
  w_sstatus(r_sstatus() & ~SSTATUS_SIE);
  w_sie(0);

  w_stvec((uint64)kernelvec);
  w_satp(0);
  asm volatile("sfence.vma" ::: "memory");

  uart_puts("before interrupt enables\n");

  w_sie(SIE_SEIE | SIE_STIE);

  uart_puts("both classes armed\n");

  w_sstatus(r_sstatus() | SSTATUS_SIE);

  uart_puts("BOTH SURVIVED\n");
  uart_puts("Before main!\n");
  main();
  uart_puts("ERROR: main returned\n");
  for (;;)
    ;
}

void
timerinit()
{
  // ask for the very first timer interrupt, TIMER_TICKS from now
  //timer = r_time() + TIMER_TICKS;
  // sbi_set_timer(timer);

  // The ky x1 supports sstc extension so we can configure timer directly
  // from S-mode
  //w_stimecmp(timer);
  uart_puts("A\n");

  timer = r_time();

  uart_puts("B\n");

  timer +=  TIMER_TICKS;

  uart_puts("C\n");

  w_stimecmp(timer);

  uart_puts("D\n");

}
