/* Delay execution in kernel space 
   It uses a busy loop internally
   and should not be used a substitute
   for sleep **if possible**
*/

#ifndef DELAY_H
#define DELAY_H

/* []delay()
   - atomic context - can be used 
     even in contexts where sleeping is not
     allowed
   - precise on 'short intervals' 
     but not on 'long intervals'
*/

#include <xv6/types.h>
#include <arch/riscv/riscv.h>

/* The RV2 timebase frequency is 10MHz:
   10,000,000 ticks/second
   1 s = 1,000,000 microseconds
   so we get 10 ticks/usec
*/

// microsecond delay
static inline void udelay(uint64 usec)
{
  uint64 start = r_time();
  uint64 ticks = usec * 10;

  while (r_time() - start < ticks)
        ;
}

// millisecond delay
static inline void mdelay(uint64 msec)
{
  udelay(1000 * msec);
}

/*
// nanosecond delay
static inline void ndelay(uint64 nsec)
{
 @NOT_IMPLEMENTED
 // udelay(DIV_ROUND_UP(nsec, 1000));
}
*/

#endif
