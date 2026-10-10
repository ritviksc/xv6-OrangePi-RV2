#ifndef UART_H
#define UART_H

#include <mmio.h>

/**
 * The RV2 UARTs follow a PXA/XScale-style UART register interface,
 * which is closely related to the familiar 8250/16550 UART programming model.
 *
 * The 10 UARTs are functionally compatible with the 16550A and 16750 industry standards.
 * Each UART supports most of the 16550A and 16750 functions as well as the following features:
 *   - DMA requests for Transmit and Receive data services
 *   - Serial infrared asynchronous interface
 *   - Non-Return to Zero (NRZ) encoding/decoding function
 *   - 64 byte Transmit/Receive FIFO buffers
 *   - Programmable Receive FIFO trigger threshold
 *   - Auto baud-rate detection
 *   - Auto flow
 *
 * NOTE: The UARTs can use DMA to transfer data to and from memory as stated above,
 * but is not supported for the xv6 driver as of now.
 *
 * For full details on the UARTs take a look at the following resource links below:
 *  - https://github.com/spacemit-com/docs-chip/blob/main/en/key_stone/k1/k1_docs/k1_usermanual/16.Low-Speed_Interface_System.md#163-uart
 *  - https://www.lammertbies.nl/comm/info/serial-uart
 */


#define UART0    		 0xd4017000UL // debug UART
#define UARTCLK_FPGA             (14750000)


/**
 * The UART control registers are memory-mapped.
 *
 * Width of each register is 32 bits - 4 bytes
 * The register shift is 2, meaning the MMIO UART registers
 * are spaced 4 bytes apart
 */
#define UART_REG_SHIFT 		2

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

/**
 * Helper macros to use for mmio operations for the serial UART 
 * and anyway for xv6 its the only UART we will use as of now.
 */
#define ReadReg(reg)        	uart_read(UART0, reg)
#define WriteReg(reg, val)  	uart_write(UART0,reg, val)


/** 
 * The UART control registers.
 * NOTE: Some have different meanings for read vs write.
 */

/**
 * Receive Buffer Register
 * In non-FIFO mode, this register holds the character(s) received by the UART Receive Shift Register.
 * If this register is configured to use fewer than 8 bits, the bits are right-justified and the most significant bits (MSbs) are zeroed.
 * Reading the register empties the register and clears the <Data Ready> field in the Line Status Register.
 * This register latches the value of the data byte at the front of the FIFO in FIFO mode.
 */
#define RBR             	0       


/**
 * Transmit Holding Register
 * This register holds the data byte(s) to be transmitted next in non-FIFO mode.
 * When the Transmit Shift Register is emptied, the contents of this register are loaded into the Transmit Shift Register
 * and the <Transmit Data Request> field in the Line Status Register is set.
 * A write to Transmit Holding Register puts data at the top of the FIFO in FIFO mode.
 * The data at the front of the FIFO is loaded into the Transmit Shift Register when the Transmit Shift Register is empty.
 */
#define THR             	0        

#define IER             	1        // interrupt enable register
#define IER_UUE			(1 << 6) // uart unit enable - without this the uart wont work!
#define IER_RX_ENABLE   	(1 << 0) // receiver interrupts
#define IER_TX_ENABLE   	(1 << 1) // transmit interrupts

/**
 * NOTE: Changing the baud rate (writing to registers Divisor Latch Low Byte Register
 * and Divisor Latch High Byte Register) is not permitted while actively transmitting or receiving data.
 * It can cause data corruption!
 */
#define UART_DLL                0       /* Out: Divisor Latch Low */
#define UART_DLM                1       /* Out: Divisor Latch High */
// #define UART_DIV_MAX            0xFFFF  /* Max divisor value */

/**
 * Each UART has 2 FIFOs: 1 Transmit and 1 Receive. 
 * The Transmit FIFO is 64 bytes deep and 8 bits wide. 
 * The Receive FIFO is 64 bytes deep and 11 bits wide. 
 * The three extra bits are used for tracking errors.
 */
#define FIFO_SIZE		64
#define FCR            		2        
#define FCR_FIFO_ENABLE 	(1 << 0)
#define FCR_FIFO_CLEAR  	(3 << 1) // clear the content of the two FIFOs

#define IIR             	2        // interrupt identification register
// bits 2:1 tell us what type of interrupt has occured
#define MODEM_INTR		0x0 // Modem Status (CTS, DSR, RI, DCD modem signals changed state).
#define TX_INTR			0x1 // Transmit FIFO requests data.
#define RX_INTR			0x2 // Received data available.
#define RX_ERR			0x3 // Receive error (Overrun, parity, framing, break)


#define LCR            		 3        // line control register
#define LCR_EIGHT_BITS  	(3 << 0)  // 8N1
#define LCR_BAUD_LATCH  	(1 << 7) // special mode to set baud rate


#define LSR             	5        // line status register
#define LSR_RX_READY    	(1 << 0) // input is waiting to be read from RHR
#define LSR_TX_IDLE     	(1 << 5) // THR can accept another character to send
// error reasons
#define LSR_OVERRUN       	(1 << 1)
#define LSR_PARITY_ERR    	(1 << 2)
#define LSR_FRAMING_ERR   	(1 << 3)
#define LSR_BREAK         	(1 << 4)

// Declarations
void            uartinit(void);
void            uartintr(void);
void            uartwrite(char [], int);
void            uartputc_sync(int);


#endif
