#include <drivers/uart/uart.h>
#include <sbi/sbi_legacy.h>

// Simple UART driver that uses SBI calls to interact with the UART device
// Useful for simple debugging

// Send a char to debug console
void 
uart_putc(char c)
{
  sbi_console_putchar((unsigned char)c);
}

// Get char from debug console
int
uart_getc(void)
{
  int c;
  if ( (c = sbi_console_getchar()) < 0)
    return -1;
  else
    return c;
}

// Display a string 
void
uart_puts(const char *s)
{
  while (*s != '\0') {
    uart_putc(*s);
    s++;
 }
}
