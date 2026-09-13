/* Hardware Watchdog Driver for OrangePi RV2 for xv6
   @references u-boot-orangepi repository
   @files
     * ky_pmic_wdt.c
*/


#include <xv6/types.h>
#include <drivers/i2c/i2c.h>
#include <drivers/pmic/pmic.h>
#include <drivers/watchdog/pmic_wdt.h>

typedef uint64 addr_t;

/* Define pmic struct, especially which 
   i2c controller it uses 
*/
struct pmic_wdt
{
  i2c_t index;
  addr_t base;
  addr_t pwr_ctrl_0;
  addr_t pwr_ctrl_2;
  addr_t alive_reg;

};

static struct pmic_wdt wdt_dev = 
{
  .index = I2C8,
  .base = SPM8821_WDT_CTRL,
  .pwr_ctrl_0 = SPM8821_PWR_CTRL0,
  .pwr_ctrl_2 = SPM8821_PWR_CTRL2,
  .alive_reg = SPM8821_ALIVE_REGISTER
};

// byte buffer
static uchar buf[1];
 
static int pmic_wdt_initialize(uint64 timeout_ms)
{
  int ret;
  uint32 timeout_val;
  uint32 reg_val;

  if (timeout_ms <= 1000) {
     timeout_val = SPM8821_WDT_TIMEOUT_1S;
  } else if (timeout_ms <= 4000) {
     timeout_val = SPM8821_WDT_TIMEOUT_4S;
  } else if (timeout_ms <= 8000) {
     timeout_val = SPM8821_WDT_TIMEOUT_8S;
  } else {
     timeout_val = SPM8821_WDT_TIMEOUT_16S;
  }

  // Clear the status
  ret = pm8xx_read(wdt_dev.index, wdt_dev.base, 1, buf, 1);
  if (ret < 0)
    return ret;

  reg_val = (uint32)buf[0];
  reg_val |= SPM8821_WDT_CLEAR_STATUS;

  buf[0] = (uchar)reg_val;

  ret = pm8xx_write(wdt_dev.index, wdt_dev.base, 1, buf, 1);
  if (ret)
    return ret;


  // Set the timeout value
  ret = pm8xx_read(wdt_dev.index, wdt_dev.base, 1, buf, 1);
  if (ret < 0)
    return ret;

  reg_val = (uint32)buf[0];
  reg_val = (reg_val & ~(0x3 << 1)) | (timeout_val << 1);

  buf[0] = (uchar)reg_val;

  ret = pm8xx_write(wdt_dev.index, wdt_dev.base, 1, buf, 1);
  if (ret)
    return ret;


  // Enable the watchdog
  ret = pm8xx_read(wdt_dev.index, wdt_dev.base, 1, buf, 1);
  if (ret < 0)
     return ret;

  reg_val = (uint32)buf[0];
  reg_val |= SPM8821_WDT_ENABLE;

  buf[0] = (uchar)reg_val;

  ret = pm8xx_write(wdt_dev.index, wdt_dev.base, 1, buf, 1);
  if (ret)
      return ret;


  // Enable watchdog reset
  ret = pm8xx_read(wdt_dev.index, wdt_dev.pwr_ctrl_0, 1, buf, 1);
  if (ret < 0)
     return ret;

  reg_val = (uint32)buf[0];
  reg_val |= SPM8821_WDT_RESET_ENABLE;

  buf[0] = (uchar)reg_val;

  ret = pm8xx_write(wdt_dev.index, wdt_dev.pwr_ctrl_0, 1, buf, 1);
  if (ret)
    return ret;

  return 0;
}

int pmic_wdt_reset()
{
  int ret;
  uint32 reg_val;

  /* Clear watchdog timer status */
  ret = pm8xx_read(wdt_dev.index, wdt_dev.base, 1, buf, 1);
  if (ret < 0) 
    return ret;

  reg_val = (uint32)buf[0];
  reg_val |= SPM8821_WDT_CLEAR_STATUS;

  buf[0] = (uchar)reg_val;

  ret = pm8xx_write(wdt_dev.index, wdt_dev.base, 1, buf, 1);
  if (ret) 
    return ret;

  ret = pm8xx_read(wdt_dev.index, wdt_dev.pwr_ctrl_0, 1, buf, 1);
  if (ret < 0)
    return ret;

  reg_val = (uint32)buf[0];
  reg_val |= 0x1;

  buf[0] = (uchar)reg_val;

  ret = pm8xx_write(wdt_dev.index, wdt_dev.pwr_ctrl_0, 1, buf, 1);
  if (ret) 
    return ret;

  return 0;

}

int pmic_wdt_start(uint64 timeout_ms)
{
  int ret = pmic_wdt_initialize(timeout_ms);
  if (ret) {
     return ret;
   }

  uint32 reg_val;

  /* Start watchdog timer */
  ret = pm8xx_read(wdt_dev.index, wdt_dev.base, 1, buf, 1);
  if (ret < 0)
     return ret;
  
  reg_val = (uint32)buf[0];
  reg_val |= SPM8821_WDT_ENABLE;

  buf[0] = (uchar)reg_val;

  ret = pm8xx_write(wdt_dev.index, wdt_dev.base, 1, buf, 1);
  if (ret) 
    return ret;

  return 0;
}

int pmic_wdt_stop()
{
  int ret;
  uint32 reg_val;

  ret = pm8xx_read(wdt_dev.index, wdt_dev.base, 1, buf, 1);
  if (ret < 0) 
    return ret;

  reg_val = (uint32)buf[0];
  reg_val &= ~SPM8821_WDT_ENABLE;

  buf[0] = (uchar)reg_val;

  ret = pm8xx_write(wdt_dev.index, wdt_dev.base, 1, buf, 1);
  if (ret) 
    return ret;

  return 0;

}

