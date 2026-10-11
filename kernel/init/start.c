#include <xv6/types.h>
#include <xv6/param.h>
#include <arch/riscv/rv2/memlayout.h>
#include <arch/riscv/riscv.h>
#include <xv6/defs.h>
#include <sbi/sbi_legacy.h>
#include <drivers/uart/sbi_uart.h>

extern void main();

// entry.S needs one stack per CPU.
__aligned(16) char stack0[4096 * NCPU];

// entry.S jumps here in supervisor mode on stack0.
__noreturn void
start()
{
  intr_off();
  w_sie(0);

  w_satp(0);
  sfence_vma();

  main();

  __unreachable;
  
}


