#ifndef PMIC_H
#define PMIC_H

#define PMIC_BASE 0x41

int pm8xx_read(i2c_t i, uint reg, int alen, uchar *buff, int len);
int pm8xx_write(i2c_t i, uint32 reg, int alen, const uchar *buff, int len);


#endif
