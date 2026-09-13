#ifndef PMIC_WDT_H
#define PMIC_WDT_H

/* Watchdog Timer Registers */
#define SPM8821_WDT_CTRL           0x44
#define SPM8821_PWR_CTRL0          0x7C
#define SPM8821_PWR_CTRL2          0x7E
#define SPM8821_PWR_CTRL2_MSK           0xff

/* Watchdog Timer Control Bits */
#define SPM8821_WDT_CLEAR_STATUS     0x1
#define SPM8821_SW_RST               0x2
#define SPM8821_WDT_RESET_ENABLE     0x80
#define SPM8821_WDT_ENABLE           0x8
#define SPM8821_WDT_TIMEOUT_1S       0x0
#define SPM8821_WDT_TIMEOUT_4S       0x1
#define SPM8821_WDT_TIMEOUT_8S       0x2
#define SPM8821_WDT_TIMEOUT_16S      0x3

#define SPM8821_ALIVE_REGISTER          0xab
#define SPM8821_ALIVE_REGISTER_MSK      0x7
#define SYS_REBOOT_FLAG_BIT             0x2

int pmic_wdt_reset();
int pmic_wdt_start(uint64 timeout_ms);
int pmic_wdt_stop();

#endif
