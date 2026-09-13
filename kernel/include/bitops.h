#ifndef BITOPS_H
#define BITOPS_H

// Standard bit operations helper
#include <mmio.h>

#define BIT(n) (1UL << n)

static inline void
clrsetbits32(volatile uint32 *addr, uint32 clear, uint32 set)
{
    uint32 x = readl(addr);
    x &= ~clear;
    x |= set;
    writel(x, addr);
}

#endif
