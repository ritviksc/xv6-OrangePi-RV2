#include <arch/riscv/rv2/memlayout.h>
#include <sbi/sbi_legacy.h>
#include <xv6/types.h>
#include <drivers/uart/uart.h>

extern uint64 time;

void
simple_trap(void)
{
  uart_puts("TIMER TRAP\n");
  // configure timer to go off again
  time += TIMER_TICKS;
  sbi_set_timer(time);
}
