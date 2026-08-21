// A minimal test point for the kernel
// This checks if xv6 is executing or not

#include <drivers/uart/uart.h>
#include <arch/riscv/rv2/memlayout.h>

void
main(void)
{
  uart_puts("DEVICE:");
  uart_puts(DEVICE_NAME);
  uart_putc('\n');

  while (1) {
    __asm__ volatile ("wfi");
  }

}
