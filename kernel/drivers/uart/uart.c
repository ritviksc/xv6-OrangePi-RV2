/* UART device driver for the OrangePi RV2. This driver will be primarily used for
 * operating the debug UART, which makes xv6 interactive.
 *
 * Based on the original xv6 UART driver for QEMU. Some 'things' are modified
 * but the two driver files should be relativley similar in style.
 * Author:  Ritvik Sharma
 * Created: 29/09/26
 *
 * Files referenced from linux-orangepi repository:
 *   - serial_reg.h 
 *   - pxa_x1.c
 */

#include <mmio.h>
#include <bitops.h>
#include <barrier.h>
#include <xv6/types.h>
#include <xv6/param.h>
#include <locks/spinlock.h>
// #include <drivers/uart/sbi_uart.h>
#include <drivers/uart/uart.h>
#include <drivers/reset/reset.h>
#include <drivers/tty/console.h>
#include <arch/riscv/rv2/memlayout.h>
// #include <arch/riscv/riscv.h>
// #include <locks/spinlock.h>
// #include <proc/proc.h>

// for sending threads to synchronize with uart "ready" interrupts.
// static struct spinlock tx_lock;
// static int tx_busy; // is the UART busy sending?
// static int tx_chan; // &tx_chan is the "wait channel"

extern volatile int panicking; // from printk.c
extern volatile int panicked;  // from printk.c

// track rx errors
static struct {
  uint64 parity;
  uint64 framing;
  uint64 overrun;
  uint64 breaks;
} uart_errors;

static struct {
  struct spinlock tx_lock;

  // output circular buffer
#define OUTPUT_BUF_SIZE (2 * FIFO_SIZE)
  char output_buf[OUTPUT_BUF_SIZE];
  uint tx_read;   // Next byte to transfer to the UART TX FIFO
  uint tx_write;  // Next position where byte will be enqueued
} output;

/** 
 * Deassert the reset for serial UART
 * to set it to a known good state.
 */
static void 
serial_uart_reset()
{ 
  reset_set(APBC_UART1_CLK_RST,BIT(2),0);
}

/**  
 * We assume that OpenSBI configures the clock
 * responsible for the serial debug UART as it is the
 * only UART we are really concerned with as of now.
 */
static void
uart_clk_enable()
{ /*__NOT_IMPLEMENTED__*/ }

/**
 * Configure the serial UART to work properly. We want the UART to be in 8N1 mode,
 * which means the data payload is 8-bits, no parity bits, and 1 stop bit.
 * Standard Baud Rate is 115200, so we must configure it as required.
 * Divisor in this case is Frequency/(16 * Baud Rate):
 *   - (UARTCLK_FPGA/(16 * 115200)) so 0x0008 where MSB = 0x00 and LSB = 0x08.
 */
void
uartinit()
{
  // Enable the UARTs clock and deassert the reset
  uart_clk_enable();
  serial_uart_reset();

  initlock(&(output.tx_lock), "uart");
  
  WriteReg(IER, IER_UUE); // clear all interrupts, but make sure the UART unit is enabled!

  // special mode to set baud rate.
  WriteReg(LCR, LCR_BAUD_LATCH);

  // LSB for baud rate of 115.2K.
  WriteReg(UART_DLL, 0x08);

  // MSB for baud rate of 115.2K.
  WriteReg(UART_DLM, 0x00);

  // leave set-baud mode,
  // and set word length to 8 bits, no parity.
  WriteReg(LCR, LCR_EIGHT_BITS);

  // reset and enable FIFOs.
  WriteReg(FCR, FCR_FIFO_ENABLE | FCR_FIFO_CLEAR);

  // enable transmit and receive interrupts.
  WriteReg(IER, IER_UUE | IER_RX_ENABLE);   // | IER_TX_ENABLE);
}

/**
 * Transmit buf[] to the output buffer. It blocks if the
 * uart is busy, so it cannot be called from
 * interrupts, only from write() system calls.
 *
void
uartwrite(char buf[], int n)
{
  acquire(&tx_lock);

  int i = 0;
  while (i < n) {
    uartstart();
  }

  release(&tx_lock);
}
*/

/**
 * Write a byte to the uart without using
 * interrupts and output buffer, for use by kernel printk() and
 * to echo characters. It spins waiting for the uart's
 * output register to be empty.
 */
void
uartputc_sync(int c)
{
 if (panicking == 0)
   push_off();

  if (panicked) {
    for (;;)
      __asm__ __volatile__ ("wfi");
  }

  // wait for UART to set Transmit Holding Empty in LSR.
  while ((ReadReg(LSR) & LSR_TX_IDLE) == 0)
    ;
  WriteReg(THR, c);

 if (panicking == 0)
   pop_off();
}

/**
 * Try to read one input character from the UART. Check for errors
 * and update counters as well for debug information.
 * Return -1 if RBR is empty 
 * Return -2 if an error occured
 * Return character otherwise
 */
static int
uartgetc()
{
   uint32 lsr = ReadReg(LSR);

   if (lsr & LSR_OVERRUN)
     uart_errors.overrun++;

   if (lsr & LSR_PARITY_ERR)
     uart_errors.parity++;

   if (lsr & LSR_FRAMING_ERR)
     uart_errors.framing++;

   if (lsr & LSR_BREAK)
     uart_errors.breaks++;

#define RX_EMPTY -1
   if (!(lsr & LSR_RX_READY))
     return RX_EMPTY;

   int c = ReadReg(RBR) & 0xff;

#define ERR_CHAR -2
   if (lsr & (LSR_PARITY_ERR | LSR_FRAMING_ERR | LSR_BREAK))
     return ERR_CHAR;
   
   return c;
}

/** 
 * Handle an uart interrupt, and we read the 
 * Interrupt Identification Register to check the 
 * cause of the interrupt.
 *
 * Since the IIR tells us about one interrupt event at
 * a time, we read the IIR until IIR[0] is cleared. 
 * If multiple interrupts are pending, the lower-priority ones are 
 * masked until the higher-priority ones are cleared.
 *
 * The default hardwired priority order from highest to lowest is:
 *   1. Receiver Line Status (Errors like Overrun, Parity, Framing, or Break)
 *   2. Received Data Available
 *   3. Character Timeout
 *   3. Transmitter Holding Register Empty
 *   4. Modem Status Change
 *
 * xv6 doesn't handle all of them as of now
 *
 * Called from devintr().
 */
void
uartintr()
{
 uint32 iir = ReadReg(IIR);
#define IRQ_TRIGGER 0
 while ((iir & 0x01) == IRQ_TRIGGER) {
    
   uint32 char_timeout = (iir >> 3) & 1; // IIR[3]
   uint32 irq_reason = (iir >> 1) & 3;   // IIR[2:1]

   // some characters remain in RX FIFO so proceed to drain them
   if (char_timeout) {
     goto rx_handler;
   }

    switch(irq_reason) {
     
      case RX_ERR:
        goto rx_handler;

rx_handler: // NOTE: goto's are generally discouraged but I feel like they work here ;)
      case RX_INTR: { // drain RX FIFO while discarding erroneous characters
        int c;
        while((c = uartgetc()) != RX_EMPTY){ 
          if (c == ERR_CHAR)
            continue;
          // consoleintr(c);
          uartputc_sync(c); // TESTING
        }
	break;
      }

      case TX_INTR:
        // accquire(&(output.tx_lock));
        // consolewrite();
        // release(&(output.tx_lock));
        break;

      case MODEM_INTR:
        // ReadReg(MSR)
        break;
    } 
  
   iir = ReadReg(IIR);
 }

}
