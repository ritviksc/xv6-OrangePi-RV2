#include <arch/riscv/rv2/memlayout.h>
#include <sbi/sbi_legacy.h>
#include <xv6/types.h>
#include <drivers/uart/uart.h>

extern uint64 timer;

void
simple_trap(void)
{
  uart_puts("TIMER TRAP\n");
  // configure timer to go off again
  timer += TIMER_TICKS;
  sbi_set_timer(timer);
}
