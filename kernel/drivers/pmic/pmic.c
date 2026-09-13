/* Power Management Integrated Circuit (PMIC) driver for the Rv2 for xv6 
   We communicate with the PMIC via i2c bus 
*/

/* @references u-boot-orangepi repository
   files
     * ky_pmic.c
     * ky_pmic.h
     * pmic.h
*/

#include <xv6/types.h>
#include <drivers/i2c/i2c.h>
#include <drivers/pmic/pmic.h>

int pm8xx_read(i2c_t i, uint32 reg, int alen, uchar *buff, int len)
{
  int ret;

  ret = i2c_read(i, PMIC_BASE, reg, alen, buff, len);
  if (ret) {
    return ret;
  }

  return 0;
}

int pm8xx_write(i2c_t i, uint32 reg, int alen, const uchar *buff, int len)
{
  int ret;

  ret = i2c_write(i, PMIC_BASE, reg, alen, buff, len);
  if (ret) {
    return ret;
   }

  return 0;
}



