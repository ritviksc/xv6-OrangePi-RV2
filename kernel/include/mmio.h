/* Helper functions to read and write from/to memory-mapped io devices */
/* Suffixes to read and write functions follow the historic linux conventions */

#ifndef MMIO_H
#define MMIO_H

#include <xv6/types.h>

/* All read/write operations are relaxed meaning a use of a barrier
   may be required if specific read/write operations need to occur
   before others
*/

// read mmio ops
static inline uint8 readb(volatile void *addr)
{
  return *((volatile uint8 *)addr);
}

static inline uint16 readw(volatile void *addr)
{
  return *((volatile uint16 *)addr);
}

static inline uint32 readl(volatile void *addr)
{
  return *((volatile uint32 *)addr);
}

static inline uint64 readq(volatile void *addr)
{
  return *((volatile uint64 *)addr);
}

// write mmio ops
static inline void writeb(uint8 value, volatile void *addr)
{
  *((volatile uint8 *)addr) = value;
}

static inline void writew(uint16 value, volatile void *addr)
{
  *((volatile uint16 *)addr) = value;
}

static inline void writel(uint32 value, volatile void *addr)
{
  *((volatile uint32 *)addr) = value;
}

static inline void writeq(uint64 value, volatile void *addr)
{
  *((volatile uint64 *)addr) = value;
}

#endif
