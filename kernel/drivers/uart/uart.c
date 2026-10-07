/*  Serial-port console device driver for the OrangePi RV2.
 *  The RV2 UART follows a PXA/XScale-style UART register interface,
 *  which is closely related to the familiar 8250/16550 UART programming model.
 *
 *  Based on the original xv6 UART driver for QEMU. Some 'things' are modified
 *  but the two driver files should be relativley similar in style.
 *  Author:  Ritvik Sharma
 *  Created: 29/09/26
 *
 *  Files referenced from linux-orangepi repository:
 *    - serial_reg.h 
 *    - pxa_x1.c
 */

#include <mmio.h>
#include <bitops.h>
#include <xv6/types.h>
#include <xv6/param.h>
#include <locks/spinlock.h>
#include <drivers/uart/uart.h>
#include <drivers/reset/reset.h>
#include <drivers/tty/console.h>
#include <arch/riscv/rv2/memlayout.h>
// #include <arch/riscv/riscv.h>
// #include <locks/spinlock.h>
// #include <proc/proc.h>


/**
 *  The UART control registers are memory-mapped.
 *
 *  Width of each register is 32 bits - 4 bytes
 *  The register shift is 2, meaning the MMIO UART registers
 *  are spaced 4 bytes apart
 */

#define UART_REG_SHIFT 2

static inline uint32
uart_read(uint64 base, uint64 reg)
{ 
  return readl((volatile void *)base + (reg << UART_REG_SHIFT));
}

static inline void 
uart_write(uint64 base, uint64 reg, uint32 val)
{
  writel(val, (volatile void *)base + (reg << UART_REG_SHIFT));
}

// Friendly macros to use for serial UART releated ops
#define ReadReg(reg)	    uart_read(UART0, reg)
#define WriteReg(reg, val)  uart_write(UART0,reg, val)    

/*  The UART control registers.
 *  Some have different meanings for read vs write.
 */
#define RHR             0        // receive holding register (for input bytes)
#define THR             0        // transmit holding register (for output bytes)

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

#define LSR             5        // line status register
#define LSR_RX_READY    (1 << 0) // input is waiting to be read from RHR
#define LSR_TX_IDLE     (1 << 5) // THR can accept another character to send

// for sending threads to synchronize with uart "ready" interrupts.
static struct spinlock tx_lock;
static int tx_busy; // is the UART busy sending?
// static int tx_chan; // &tx_chan is the "wait channel"

extern volatile int panicking; // from printk.c
extern volatile int panicked;  // from printk.c


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
{ /* __NOT_IMPLEMENTED__ */ }

/**
 * Configure the UART to work properly. We want the UART to be in 8N1 mode,
 * which means the data payload is 8-bits, no parity bits, and 1 stop bit.
 * Standard Baud Rate is 115200, so we must configure it as required.
 * Divisor in this case is Frequency/(16 * Baud Rate):
 *   - (14750000/(16 * 115200)) so 0x0008 where MSB = 0x00 and LSB = 0x08.
 */
void
uartinit()
{
  // enable the UART clock
  uart_clk_enable();

  // deassert the reset
  serial_uart_reset();
 
  // disable interrupts.
  WriteReg(IER, 0x00);

  // special mode to set baud rate.
  WriteReg(LCR, LCR_BAUD_LATCH);

  // LSB for baud rate of 115.2K.
  WriteReg(0, 0x08); // DLL

  // MSB for baud rate of 115.2K.
  WriteReg(1, 0x00); // DLM

  // leave set-baud mode,
  // and set word length to 8 bits, no parity, 1 stop bit.
  WriteReg(LCR, LCR_EIGHT_BITS);

  // reset and enable FIFOs.
  WriteReg(FCR, FCR_FIFO_ENABLE | FCR_FIFO_CLEAR);

  // enable transmit and receive interrupts.
  WriteReg(IER, IER_TX_ENABLE | IER_RX_ENABLE);

  initlock(&tx_lock, "uart");
}

/**
 *  transmit buf[] to the uart. it blocks if the
 *  uart is busy, so it cannot be called from
 *  interrupts, only from write() system calls.
 */
void
uartwrite(char buf[], int n)
{
  acquire(&tx_lock);

  int i = 0;
  while (i < n) {
    while (tx_busy != 0) {
      // wait for a UART transmit-complete interrupt
      // to set tx_busy to 0.
      // sleep(&tx_chan, &tx_lock);
    }

    WriteReg(THR, buf[i]);
    i += 1;
    tx_busy = 1;
  }

  release(&tx_lock);
}

/**
 * Write a byte to the uart without using
 * interrupts, for use by kernel printk() and
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
      ;
  }

  // wait for UART to set Transmit Holding Empty in LSR.
  while ((ReadReg(LSR) & LSR_TX_IDLE) == 0)
    ;
  WriteReg(THR, c);

 if (panicking == 0)
   pop_off();
}

/**
 * Try to read one input character from the UART.
 * Return -1 if none is waiting.
 */
static int
uartgetc()
{
  // is input ready?
  if (ReadReg(LSR) & LSR_RX_READY) {
    return ReadReg(RHR) & 0xFF;
  } else {
    return -1;
  }
}

/** 
 * Handle a uart interrupt, raised because input has
 * arrived, or the uart is ready for more output, or
 * both. 
 * Called from devintr().
 */
void
uartintr()
{
  ReadReg(ISR); // acknowledge the interrupt
	
  /*
  acquire(&tx_lock);
  if (ReadReg(LSR) & LSR_TX_IDLE) {
    // UART finished transmitting; wake up sending thread.
    tx_busy = 0;
    // wakeup(&tx_chan);
  }
  release(&tx_lock);
  */

 // TEST
  int c;
  while ((c = uartgetc()) >= 0)
        uartputc_sync(c);
}
