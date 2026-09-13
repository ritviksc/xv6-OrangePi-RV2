#ifndef I2C_H
#define I2C_H

#define I2C_LEN 10 /* Number of i2c controllers */

/* ri2c hardware controller base address*/
#define I2C0_BASE  0xd4010800UL
#define I2C1_BASE  0xd4011000UL
#define I2C2_BASE  0xd4012000UL
#define I2C3_BASE  0xf0614000UL
#define I2C4_BASE  0xd4012800UL
#define I2C5_BASE  0xd4013800UL
#define I2C6_BASE  0xd4018800UL
#define I2C7_BASE  0xd401d000UL
#define I2C8_BASE  0xd401d800UL 

#define RI2C0_BASE 0xc0887000UL  

/* Shall the current transfer have a start/stop condition? */
#define I2C_COND_NORMAL         0
#define I2C_COND_START          1
#define I2C_COND_STOP           2

/* Shall the current transfer be ack/nacked or being waited for it? */
#define I2C_ACKNAK_WAITACK      1
#define I2C_ACKNAK_SENDACK      2
#define I2C_ACKNAK_SENDNAK      4

/* Specify who shall transfer the data (controller or device) */
#define I2C_READ                0
#define I2C_WRITE               1

/* ----- Control register bits ---------------------------------------- */

/* ICR bit 0 --> START
   ICR bit 1 --> STOP
   ICR bit 2 --> ACKNAK
   ICR bit 3 --> TB
*/

#define ICR_START       0x1             /* start bit */
#define ICR_STOP        0x2             /* stop bit */
#define ICR_ACKNAK      0x4             /* send ACK(0) or NAK(1) */
#define ICR_TB          0x8             /* transfer byte bit */
#define ICR_MA          BIT(12)         /* master abort */
#define ICR_SCLE        BIT(13)         /* master clock enable, mona SCLEA */
#define ICR_IUE         BIT(14)         /* unit enable */
#define ICR_GCD         BIT(21)         /* general call disable */
#define ICR_ITEIE       BIT(19)         /* enable tx interrupts */
#define ICR_IRFIE       BIT(20)         /* enable rx interrupts, mona: DRFIE */
#define ICR_BEIE        BIT(22)         /* enable bus error ints */
#define ICR_SSDIE       BIT(24)         /* slave STOP detected int enable */
#define ICR_ALDIE       BIT(18)         /* enable arbitration interrupt */
#define ICR_SADIE       BIT(23)         /* slave address detected int enable */
#define ICR_UR          BIT(10)         /* unit reset */
#define ICR_SM          (0x0)           /* Standard Mode */
#define ICR_FM          BIT(8)          /* Fast Mode */
#define ICR_MODE_MASK   (0x300)         /* Mode mask */

/* ----- Status register bits ----------------------------------------- */

#define ISR_RWM         BIT(13)         /* read/write mode */
#define ISR_ACKNAK      BIT(14)         /* ack/nak status */
#define ISR_UB          BIT(15)         /* unit busy */
#define ISR_IBB         BIT(16)         /* bus busy */
#define ISR_SSD         BIT(24)         /* slave stop detected */
#define ISR_ALD         BIT(18)         /* arbitration loss detected */
#define ISR_ITE         BIT(19)         /* tx buffer empty */
#define ISR_IRF         BIT(20)         /* rx buffer full */
#define ISR_GCAD        BIT(21)         /* general call address detected */
#define ISR_SAD         BIT(23)         /* slave address detected */
#define ISR_BED         BIT(22)         /* bus error no ACK/NAK */

#define I2C_ISR_INIT    0x1FDE000

/* The bus can operate at different speeds (measured in Hz, typically 100KHz
   or 400KHz). We can use fast mode (400kHz) but 100KHz is sufficent for inital port 
*/
#define CONFIG_SYS_I2C_SPEED 100000

#if (CONFIG_SYS_I2C_SPEED == 400000)
#define I2C_ICR_INIT (ICR_FM | ICR_BEIE | ICR_IRFIE | ICR_ITEIE | ICR_GCD \
                | ICR_SCLE)
#else
#define I2C_ICR_INIT    (ICR_BEIE | ICR_IRFIE | ICR_ITEIE | ICR_GCD | ICR_SCLE)
#endif


typedef enum {
  I2C0,
  I2C1,
  I2C2,
  I2C3,
  I2C4,
  I2C5,
  I2C6,
  I2C7,
  I2C8,
  RI2C0
} i2c_t;

void i2c_init(i2c_t i);
int i2c_read(i2c_t i, uchar chip, uint32 addr, int alen, uchar *buffer, int len);
int i2c_write(i2c_t i, uchar chip, uint32 addr, int alen, const uchar *buffer, int len);

#endif
