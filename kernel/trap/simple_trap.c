#include <arch/riscv/rv2/memlayout.h>
#include <sbi/sbi_legacy.h>
#include <drivers/uart/uart.h>

void
simple_trap(void)
{
  uart_puts("TIMER TRAP");
  // configure timer to go off again!
  sbi_set_timer(TIMER_TICKS);
}
