/* SOC Watchdog Driver for the OrangePi RV2
   References @u-boot-orangepi repository - branch V.2022.10-ky
   Files:
    * wdt.h
    * ky_wdt.c
*/

#include <mmio.h>
#include <xv6/types.h>
#include <drivers/reset/reset.h>
#include <drivers/watchdog/wdt.h>
#include <arch/riscv/rv2/memlayout.h>

/* Register offsets relative to the base address of the WDT */
/* Registers have a width of 4 bytes - 32 bits */
#define WDT_ENABLE_OFFSET   0xB8
#define WDT_TIMEOUT_OFFSET  0xBC
#define WDT_RESET_OFFSET    0xC8
#define WDT_STATUS_OFFSET   0xC0
#define WDT_WSAR_OFFSET     0xB4
#define WDT_WFAR_OFFSET     0xB0

/* Watchdog Timer Control Bits */
#define WDT_CLEAR_STATUS     0x0
#define WDT_RESET_ENABLE     0x1
#define WDT_ENABLE           0x3
#define WDT_TIMEOUT          0x1

/* WDT Start Register and its enable bit */
#define WDT_START_OFFSET    0xd4050000UL 
#define WDT_START_ENABLE    (1UL << 4) // bit at index 4 needs to be 1
#define WDT_CLK_FREQ        256

/* WDT key values */
#define WDT_WFAR_KEY        0xbaba
#define WDT_WSAR_KEY        0xeb10

/* CCU & Reset controller values */
/* @TODO: make reset and ccu drivers */
#define MPMU_BASE       0xd4050000UL
#define MPMU_WDTPCR     0x200

#define WDT_CLK_MASK    0x3
#define WDT_RESET_BIT   (1UL << 2)

/* Unlock write-protected registers in the wdt
   It expects two key values in sequence, hence use a fence!
   We must do this for every write operation
*/
static void wdt_write_access(uint64 base)
{
  // write sequence values
  writel(WDT_WFAR_KEY, (void *)(base + WDT_WFAR_OFFSET));
  __asm__ volatile("fence iorw, iorw" ::: "memory");
  writel(WDT_WSAR_KEY, (void *)(base + WDT_WSAR_OFFSET));
  __asm__ volatile("fence iorw, iorw" ::: "memory");
}

/* Wrapper function to write to specified wdt register
   We must write_access() first before modifying a register
*/
static void wdt_write(uint32 val, void *reg, uint64 base)
{
  wdt_write_access(base);
  writel(val, reg);
  __asm__ volatile("fence iorw, iorw" ::: "memory");
}

/* Enable WDT clock */
static void wdt_clk_enable()
{
  uint32 reg;
  reg = readl((void *)MPMU_BASE + MPMU_WDTPCR);
  reg &= ~WDT_CLK_MASK;
  reg |= WDT_CLK_MASK;
  writel(reg, (void *)MPMU_BASE + MPMU_WDTPCR);
}

/* Deassert WDT reset */
static void wdt_reset_deassert()
{
  uint32 reg;
  reg = readl((void *)MPMU_BASE + MPMU_WDTPCR);
  reg &= ~WDT_RESET_BIT;
  writel(reg, (void *)MPMU_BASE + MPMU_WDTPCR);
}

/* Initialise the watchdog
   Enable its clock, and deassert the reset
   Also, configure its registers with proper params
*/
static void wdt_init(uint64 timeout_ms)
{
  
  // program the wdt with proper counter
  uint64 counter = (timeout_ms * WDT_CLK_FREQ) / 1000;

  /* We need the clock controller to send a clock pulse to the
     watchdog so it can operate */
  /* The mpmu register in the ccu controls this */
  wdt_clk_enable();  

  /* Also need to deassert the reset */
  // [RESET_WDT]     = { MPMU_WDTPCR, BIT(2), 0, BIT(2), RST_BASE_TYPE_MPMU }
  // Write 0 to 2nd bit to deassert, write 1 to reassert
  wdt_reset_deassert();

  /* Set watchdog timer parameters */
  wdt_write(WDT_CLEAR_STATUS, (void *)(WDT_BASE + WDT_STATUS_OFFSET), WDT_BASE);
  wdt_write(counter, (void *)(WDT_BASE + WDT_TIMEOUT_OFFSET), WDT_BASE);
  wdt_write(WDT_ENABLE, (void *)(WDT_BASE + WDT_ENABLE_OFFSET), WDT_BASE);
  wdt_write(WDT_RESET_ENABLE, (void *)(WDT_BASE + WDT_RESET_OFFSET), WDT_BASE);
}

/* Start the watchdog */
void wdt_start(uint64 timeout_ms)
{
  wdt_init(timeout_ms);
    
  /* Start watchdog timer */
  uint32 reg = readl((void *)WDT_START_OFFSET);
  writel(reg | WDT_START_ENABLE, (void *)WDT_START_OFFSET);
}

/* Reset watchdog, restoring watchdog counter to value
   set in wdt_start
*/
void wdt_reset()
{
  /* Clear watchdog timer status only, do not set timeout */
  wdt_write(WDT_CLEAR_STATUS, (void *)(WDT_BASE + WDT_STATUS_OFFSET), WDT_BASE);
  wdt_write(WDT_RESET_ENABLE, (void *)(WDT_BASE + WDT_RESET_OFFSET), WDT_BASE);
}

/* Stop the watchdog, disabling it
   To start watchdog once again, use wdt_start
*/
void wdt_stop()
{
  // Stop the watchdog timer by clearing the enable bit.
  wdt_write(0x0, (void *)(WDT_BASE + WDT_ENABLE_OFFSET), WDT_BASE);
}

/* Make watchdog go off now, can be used as 
   a system reset trigger 
*/
void wdt_expire_now()
{
  wdt_start(0);
}


