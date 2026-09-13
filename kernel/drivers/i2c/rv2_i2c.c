/* i2c (inter-integrated circuit) driver for the OrangePi RV2 for xv6
   This driver references the following files from u-boot-orange pi repository
     * ky_i2c.c
     * ky_i2c.h
     * i2c.h
	
   @author: Ritvik Sharma
*/

#include <mmio.h>
#include <bitops.h>
#include <xv6/types.h>
#include <xv6/delay.h>
#include <drivers/i2c/i2c.h>
#include <drivers/reset/reset.h>

/* We write required data to the i2c controller which is memory-mapped in the CPU memory space.
   The controller is responsible for sending the data through the i2c bus to specified on-board/external 
   device.
   The RV2 has 10 i2c controllers avaliable to use.
   NOTE - the i2c controller registers have a width of 4 bytes - 32 bits so use
   readl() and writel().

   A specfic i2c controller needs a clock signal and deassert - we will assume the CCU configures  
   the controller due to u-boot, and only deassert the reset for now, as there is no universal ccu
   driver as of now for xv6.
   
*/

/* There are two modes the controller can be in - read mode or write mode.

   In read mode, the controller requests data from a specfic device.
   In write mode, the controller writes data to a specfic device.

   In any case, the controller initiates the request by triggering the START
   condition. The serial data line goes LOW, while the serial clock line is 
   HIGH. Shortly after the serial clock line goes LOW as well and bits are clocked.
   All the devices on the i2c bus will now pay attention to see which
   device needs to be involved in the transaction. The controller will send the 
   address of the device it required - this can be a 7-bit address or 10-bit address.
   This means that we can have 128-1024 devices connected to one i2c bus, but in
   reality not all addresses are used.

   It will also send a config bit which tells the device if it needs to be read from
   or written to.
 	
    __________________________________________________________________
   |BIT 6 | BIT 5 | BIT 4 | BIT 3 | BIT 2 | BIT 1 | BIT 0 | | R/W BIT |

   Typically bit 1 -> read mode & bit 0 -> write mode. After this sequence
   actual data that needs to be read/write will be transmitted if device sends
   a ACK bit back to controller by pulling data line LOW.
 
*/

/* All transfers are described by this data structure */
/* Data is sent 8 bits at a time over the serial data line */
struct i2c_msg {
  uint8 condition;
  uint8 acknack;
  uint8 direction;
  uint8 data;
};

/* Registers start from offset 0 from base addr, and in
   this case are each 4 bytes apart */
struct i2c {
  uint32 icr;      // control register
  uint32 isr;      // status register
  uint32 isar;     // slave address register (if configured)
  uint32 idbr;     // data buffer register (where data is stored)
  uint32 ilcr;     //
  uint32 iwcr;     //
  uint32 irst_cyc; //
  uint32 ibmr;     // 
};  

/* Array of all i2c controllers and their base addresses */
static struct i2c *i2c_bases[I2C_LEN] =
{
  (struct i2c *)I2C0_BASE,
  (struct i2c *)I2C1_BASE,
  (struct i2c *)I2C2_BASE,
  (struct i2c *)I2C3_BASE,
  (struct i2c *)I2C4_BASE,
  (struct i2c *)I2C5_BASE,
  (struct i2c *)I2C6_BASE,
  (struct i2c *)I2C7_BASE,
  (struct i2c *)I2C8_BASE,
  (struct i2c *)RI2C0_BASE
};


/* deassert reset for a i2c controller */
static void deassert_i2c(struct i2c *base)
{
  switch((uint64)base) {
  
    case I2C0_BASE:	  
      reset_set(APBC_TWSI0_CLK_RST, BIT(2), BIT(2));
      break;

    case I2C1_BASE:
      reset_set(APBC_TWSI1_CLK_RST, BIT(2), BIT(2));
      break;

    case I2C2_BASE:
      reset_set(APBC_TWSI2_CLK_RST, BIT(2), BIT(2));
      break;

    case I2C3_BASE:
      reset_set(APBC2_TWSI3_CLK_RST, BIT(2), BIT(2));
      break;

    case I2C4_BASE:
      reset_set(APBC_TWSI4_CLK_RST, BIT(2), BIT(2));
      break;

    case I2C5_BASE:
      reset_set(APBC_TWSI5_CLK_RST, BIT(2), BIT(2));
      break;

    case I2C6_BASE:
      reset_set(APBC_TWSI6_CLK_RST, BIT(2), BIT(2));
      break;

    case I2C7_BASE:
      reset_set(APBC_TWSI7_CLK_RST, BIT(2), BIT(2));
      break;

    case I2C8_BASE:
      reset_set(APBC_TWSI8_CLK_RST, BIT(2), BIT(2));
      break;

    // case RI2C0_BASE: @NOT_IMPLEMENTED

   default:
     break;
  }

}

/* Set bus speed using controller */
static void i2c_set_bus_speed(struct i2c *base, uint32 speed)
{
  uint32 val;

  if (speed > 100000)
    val = ICR_FM; // 0b10
  else
    val = ICR_SM; // 0b00
  clrsetbits32(&base->icr, ICR_MODE_MASK, val);
}


/* Set i2c controller to a known good state before
   using it (again). We use delays to let controller
   'settle down' after configuring it.
*/
static void i2c_reset(struct i2c *base)
{
  uint32 icr_mode;

  /* Save bus mode (standard or fast speed) for later use */
  icr_mode = readl(&base->icr) & ICR_MODE_MASK;
  writel(readl(&base->icr) & ~ICR_IUE, &base->icr); /* disable unit */
  writel(readl(&base->icr) | ICR_UR, &base->icr);   /* reset the unit */
  udelay(100); // 100 us delay
  writel(readl(&base->icr) & ~ICR_IUE, &base->icr); /* disable unit */

  writel(0x00, &base->isar); /* set our slave address */

  /* set control reg values */
  writel(I2C_ICR_INIT | icr_mode, &base->icr);
  writel(I2C_ISR_INIT, &base->isr); /* set clear interrupt bits */
  writel(readl(&base->icr) | ICR_IUE, &base->icr); /* enable unit */
  udelay(1e0); // 1 us delay
}

void i2c_init(i2c_t i)
{
  struct i2c *base = i2c_bases[i];
  deassert_i2c(base);
  i2c_set_bus_speed(base,100000);
  i2c_reset(base);
}


/*
 * isr_set_cleared: - wait until certain bits of the I2C status register
 *                        are set and cleared
 *
 * @return: 1 in case of success, 0 means timeout (no match within 10 ms).
 */
static int isr_set_cleared(struct i2c *base, uint32 set_mask, uint32 cleared_mask)
{
  int timeout = 1000;
  uint32 isr;
  do {
    isr = readl(&base->isr);
    udelay(10);
    if (timeout-- < 0)
      return 0;
  } while (((isr & set_mask) != set_mask) || ((isr & cleared_mask) != 0));

   return 1;
}

/*
 * i2c_transfer: - Transfer one byte over the i2c bus
 *
 * This function can tranfer a byte over the i2c bus in both directions.
 * It is used by the public API functions.
 *
 * @return:  0: transfer successful
 *          -1: message is empty
 *          -2: transmit timeout
 *          -3: ACK missing
 *          -4: receive timeout
 *          -5: illegal parameters
 *          -6: bus is busy and couldn't be aquired
 */
static int i2c_transfer(struct i2c *base, struct i2c_msg *msg)
{
  int ret;

  if (!msg)
    goto transfer_error_msg_empty;

  switch (msg->direction) {
    case I2C_WRITE:
      /* check if bus is not busy */
      if (!isr_set_cleared(base, 0, ISR_IBB))
        goto transfer_error_bus_busy;

     /* start transmission */
     writel(readl(&base->icr) & ~ICR_START, &base->icr);
     writel(readl(&base->icr) & ~ICR_STOP, &base->icr);
     writel(msg->data, &base->idbr);
     if (msg->condition == I2C_COND_START)
        writel(readl(&base->icr) | ICR_START, &base->icr);
     if (msg->condition == I2C_COND_STOP)
        writel(readl(&base->icr) | ICR_STOP, &base->icr);
     if (msg->acknack == I2C_ACKNAK_SENDNAK)
        writel(readl(&base->icr) | ICR_ACKNAK, &base->icr);
     if (msg->acknack == I2C_ACKNAK_SENDACK) 
        writel(readl(&base->icr) & ~ICR_ACKNAK, &base->icr);
     writel(readl(&base->icr) & ~ICR_ALDIE, &base->icr);
     writel(readl(&base->icr) | ICR_TB, &base->icr);

     /* transmit register empty? */
     if (!isr_set_cleared(base, ISR_ITE, 0))
       goto transfer_error_transmit_timeout;

     /* clear 'transmit empty' state */
     if (!isr_set_cleared(base, ISR_ITE, 0))
       goto transfer_error_transmit_timeout;

     /* clear 'transmit empty' state */
     writel(readl(&base->isr) | ISR_ITE, &base->isr);

     /* wait for ACK from slave */
     if (msg->acknack == I2C_ACKNAK_WAITACK)
       if (!isr_set_cleared(base, 0, ISR_ACKNAK))
          goto transfer_error_ack_missing;

     break;


   case I2C_READ:

     /* check if bus is not busy */
     if (!isr_set_cleared(base, 0, ISR_IBB))
       goto transfer_error_bus_busy;

     /* start receive */
     writel(readl(&base->icr) & ~ICR_START, &base->icr);
     writel(readl(&base->icr) & ~ICR_STOP, &base->icr);
     if (msg->condition == I2C_COND_START)
       writel(readl(&base->icr) | ICR_START, &base->icr);
     if (msg->condition == I2C_COND_STOP)
       writel(readl(&base->icr) | ICR_STOP, &base->icr);
     if (msg->acknack == I2C_ACKNAK_SENDNAK)
        writel(readl(&base->icr) | ICR_ACKNAK, &base->icr);
     if (msg->acknack == I2C_ACKNAK_SENDACK)
        writel(readl(&base->icr) & ~ICR_ACKNAK, &base->icr);
     writel(readl(&base->icr) & ~ICR_ALDIE, &base->icr);
     writel(readl(&base->icr) | ICR_TB, &base->icr);

     /* receive register full? */
     if (!isr_set_cleared(base, ISR_IRF, 0))
        goto transfer_error_receive_timeout;

      msg->data = readl(&base->idbr);

      /* clear 'receive empty' state */
      writel(readl(&base->isr) | ISR_IRF, &base->isr);
       break;

   default:
     goto transfer_error_illegal_param;


   }

   return 0;

transfer_error_msg_empty:
  ret = -1;
  goto i2c_transfer_finish;

transfer_error_transmit_timeout:
  ret = -2;
  goto i2c_transfer_finish;

transfer_error_ack_missing:
  ret = -3;
  goto i2c_transfer_finish;

transfer_error_receive_timeout:
  ret = -4;
  goto i2c_transfer_finish;

transfer_error_illegal_param:
  ret = -5;
  goto i2c_transfer_finish;

transfer_error_bus_busy:
  ret = -6;
  goto i2c_transfer_finish;

i2c_transfer_finish:
  i2c_reset(base);
  return ret;

}

/* Read data from a chip where data comes from a particular 
   register within the chip and store it in a buffer.
   @return -1 for error
*/
static int __i2c_read(struct i2c *base, uchar chip, uint8 *addr, int alen, uchar *buffer, int len)
{

  struct i2c_msg msg;

  if (len == 0)
    return -1;
  
  i2c_reset(base);

  /* chip address write */
  msg.condition = I2C_COND_START;
  msg.acknack   = I2C_ACKNAK_WAITACK;
  msg.direction = I2C_WRITE;
  msg.data = (chip << 1);
  msg.data &= 0xFE;
  if (i2c_transfer(base, &msg))
     return -1;

  /*
   * send memory address bytes;
   * alen defines how much bytes we have to send.
   */
  while (--alen >= 0) {
    msg.condition = I2C_COND_NORMAL;
    msg.acknack   = I2C_ACKNAK_WAITACK;
    msg.direction = I2C_WRITE;
    msg.data      = addr[alen];
    if (i2c_transfer(base, &msg))
      return -1;
  }

  /* start read sequence */
  msg.condition = I2C_COND_START;
  msg.acknack   = I2C_ACKNAK_WAITACK;
  msg.direction = I2C_WRITE;
  msg.data      = (chip << 1);
  msg.data     |= 0x01;
  if (i2c_transfer(base, &msg))
     return -1;

  /* read bytes; send NACK at last byte */
  while (len--) {
    if (len == 0) {
       msg.condition = I2C_COND_STOP;
       msg.acknack   = I2C_ACKNAK_SENDNAK;
     } else {
         msg.condition = I2C_COND_NORMAL;
         msg.acknack   = I2C_ACKNAK_SENDACK;
    }

    msg.direction = I2C_READ;
    msg.data      = 0x00;
    if (i2c_transfer(base, &msg))
                        return -1;
    *buffer = msg.data;
     buffer++;
  }

  i2c_reset(base);

  return 0;
}

/*
 * i2c_read: - Read multiple bytes from an i2c device
 *
 * The higher level routines take into account that this function is only
 * called with len < page length of the device (see configuration file)
 *
 * @chip:      address of the chip which is to be read
 * @addr:      i2c data address within the chip
 * @alen:      length of the i2c data address (1 OR 2 bytes)
 * @buffer:    where to write the data
 * @len:       how many bytes do we want to read

 */
int i2c_read(i2c_t i, uchar chip, uint32 addr, int alen, uchar *buffer, int len)
{
  struct i2c *base = i2c_bases[i];
  uint8 addr_bytes[4];

  addr_bytes[0] = (addr >> 0) & 0xFF;
  addr_bytes[1] = (addr >> 8) & 0xFF;
  addr_bytes[2] = (addr >> 16) & 0xFF;
  addr_bytes[3] = (addr >> 24) & 0xFF;

  return __i2c_read(base, chip, addr_bytes, alen, buffer, len);
}

static int __i2c_write(struct i2c *base, uchar chip, uint8 *addr, int alen, const uchar *buffer, int len)
{
   struct i2c_msg msg;
   i2c_reset(base);

   /* chip address write */
   msg.condition = I2C_COND_START;
   msg.acknack   = I2C_ACKNAK_WAITACK;
   msg.direction = I2C_WRITE;
   msg.data = (chip << 1);
   msg.data &= 0xFE;
   if (i2c_transfer(base, &msg))
      return -1;

   /*
    * send memory address bytes;
    * alen defines how much bytes we have to send.
    */
   while (--alen >= 0) {
     msg.condition = I2C_COND_NORMAL;
     msg.acknack   = I2C_ACKNAK_WAITACK;
     msg.direction = I2C_WRITE;
     msg.data      = addr[alen];
     if (i2c_transfer(base, &msg))
        return -1;
   }

  /* write bytes; send NACK at last byte */
  while (len--) {

    if (len == 0)
      msg.condition = I2C_COND_STOP;
    else
      msg.condition = I2C_COND_NORMAL;

    msg.acknack   = I2C_ACKNAK_WAITACK;
    msg.direction = I2C_WRITE;
    msg.data      = *(buffer++);

    if (i2c_transfer(base, &msg))
        return -1;
 }

  i2c_reset(base);

  return 0;
}

/*
 * ky_i2c_write: -  Write multiple bytes to an i2c device
 *
 * The higher level routines take into account that this function is only
 * called with len < page length of the device (see configuration file)
 *
 * @chip:       address of the chip which is to be written
 * @addr:       i2c data address within the chip
 * @alen:       length of the i2c data address (1..2 bytes)
 * @buffer:     where to find the data to be written
 * @len:        how much byte do we want to read
 * @return:     0 in case of success
 */
int i2c_write(i2c_t i, uchar chip, uint32 addr, int alen, const uchar *buffer, int len)
{
  struct i2c *base = i2c_bases[i];
  uint8 addr_bytes[4];

  addr_bytes[0] = (addr >> 0) & 0xFF;
  addr_bytes[1] = (addr >> 8) & 0xFF;
  addr_bytes[2] = (addr >> 16) & 0xFF;
  addr_bytes[3] = (addr >> 24) & 0xFF;

  return __i2c_write(base, chip, addr_bytes, alen, buffer, len);
}

