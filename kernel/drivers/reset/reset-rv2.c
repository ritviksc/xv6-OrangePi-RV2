/* Reset driver for RV2 on-board devices tailored for xv6 */

/* References u-boot-orangepi repository
   branch: v2022.10-ky
   files:
     * reset-ky-x1.c
     * reset-uclass.h
     * reset.h
*/

/* It seems like the ccu and reset regions are shared
   as well as registers, meaning specifc bits within registers
   control if we reset or enable clock for a device 
*/

// Width of registers is 4 bytes - 32 bits

#include <xv6/mmio.h>
#include <xv6/types.h>
#include <drivers/reset/reset.h>

/* We need to send a reset signal to a specfic on-board device
   To do this we need to find what register in a specfic reset controller region
   controls the device, and what bit(s) to manipulate. A specifc bitmask will be
   used to either apply a reassert or deassert to the region of bits (mask).
   This can be visualised as a struct which linux/u-boot uses in the standard driver:
   struct ky_reset_signal {
        u32 offset; // register offsert w.r.t to a region base in the reset controller
        u32 mask;   // bitmask - region of bits we manipulate for a specfic device
        u32 deassert_val;  // what those bits should be set to release device
        u32 assert_val;    // what those bits should be set to retain device
   };
*/

void reset_set(uint32 offset, uint32 mask, uint32 set)
{
  uint32 val;
  val = readl((void *) RESET_BASE + offset); // read current state of register
  val &= ~mask; // clear required region
  val |= set; // set region
  writel(val, (void *)RESET_BASE + offset); // update state of register
}
