#include <arch/riscv/rv2/memlayout.h>
#include <drivers/uart/uart.h>
#include <xv6/types.h>

// the UART control registers are memory-mapped
// at address UART0. this macro returns the
// address of one of the registers.

// width of each register is 32 bits - 4 bytes
// reg shift is 2, meaning the MMIO uart registers
// are 4 bytes apart
#define Reg(reg) ((volatile uint32*)(UART0 + (reg << 2)))

#define ReadReg(reg)     (*(Reg(reg)))
#define WriteReg(reg, v) (*(Reg(reg)) = (v))

// the UART control registers.
// some have different meanings for read vs write.
// see serial_reg.h in the linux repository
#define UART_RX             0        // receive holding register (for input bytes)
#define UART_TX             0        // transmit holding register (for output bytes)

#define IER             1        // interrupt enable register
#define IER_RX_ENABLE   (1 << 0) // receiver interrupts
#define IER_TX_ENABLE   (1 << 1) // transmit interrupts

#define FCR             2        // FIFO control register
#define FCR_FIFO_ENABLE (1 << 0)
#define FCR_FIFO_CLEAR  (3 << 1) // clear the content of the two FIFOs
#define ISR             2        // interrupt status register

#define LCR             3        // line control register
#define LCR_EIGHT_BITS  (3 << 0)
#define LCR_BAUD_LATCH  (1 << 7) // special mode to set baud rate

#define UART_LSR        5       /* In:  Line Status Register */
#define UART_LSR_FIFOE          0x80 /* Fifo error */
#define UART_LSR_TEMT           0x40 /* Transmitter empty */
#define UART_LSR_THRE           0x20 /* Transmit-hold-register empty */
#define UART_LSR_BI             0x10 /* Break interrupt indicator */
#define UART_LSR_FE             0x08 /* Frame error indicator */
#define UART_LSR_PE             0x04 /* Parity error indicator */
#define UART_LSR_OE             0x02 /* Overrun error indicator */
#define UART_LSR_DR             0x01 /* Receiver data ready */
#define UART_LSR_BRK_ERROR_BITS (UART_LSR_BI|UART_LSR_FE|UART_LSR_PE|UART_LSR_OE)


// Send a char to console
void
uart_putc(char c)
{
  while (!(ReadReg(UART_LSR) & UART_LSR_THRE))
    ;

  WriteReg(UART_TX, c);
}

// Get char from console
int
uart_getc(void)
{
  while (!(ReadReg(UART_LSR) & UART_LSR_DR))
    ;

  int c = ReadReg(UART_RX);
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

// @empty
void uart_init()
{
  /* We don't need to initalise the UART as 
     openSBI takes care of that for us */
}

