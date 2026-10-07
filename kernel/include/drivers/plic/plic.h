#ifndef PLIC_H
#define PLIC_H

/*
 * The OrangePi RV2's PLIC **is** compatible with the 
 * SiFive PLIC.
 * Author: Ritvik Sharma
 * The PLIC driver module references following files from linux-orangepi repository
 *   - irq-sifive-plic.c
 */

#include <xv6/param.h>

/*
 * RV2 PLIC memory map
 *
 * PLIC base: 0xE0000000
 *
 * The RV2 implements 159 interrupt sources:
 *   source 0     : reserved / no interrupt
 *   sources 1-159: valid interrupt source IDs
 *
 * Priority registers:
 *   Each source has one 32-bit priority register.
 *
 *   base + 0x000000: reserved (source 0)
 *   base + 0x000004: source 1 priority
 *   base + 0x000008: source 2 priority
 *   ...
 *   base + 0x00027C: source 159 priority
 *
 * Pending registers:
 *   One bit per interrupt source, packed into 32-bit words.
 *
 *   base + 0x001000: pending sources   0-31
 *   base + 0x001004: pending sources  32-63
 *   base + 0x001008: pending sources  64-95
 *   base + 0x00100C: pending sources  96-127
 *   base + 0x001010: pending sources 128-159
 *
 * S-mode interrupt-enable registers:
 *
 *   K1 S-mode enable base   = 0x002080
 *   K1 per-hart stride      = 0x000100
 *
 *   hart 0:
 *     base + 0x002080: enable sources   0-31
 *     base + 0x002084: enable sources  32-63
 *     base + 0x002088: enable sources  64-95
 *     base + 0x00208C: enable sources  96-127
 *     base + 0x002090: enable sources 128-159
 *
 *   hart 1:
 *     base + 0x002180: enable sources   0-31
 *     base + 0x002184: enable sources  32-63
 *     ...
 *     base + 0x002190: enable sources 128-159
 *
 *   ...
 *
 *   hart 7:
 *     base + 0x002780: enable sources   0-31
 *     base + 0x002784: enable sources  32-63
 *     ...
 *     base + 0x002790: enable sources 128-159
 *
 * S-mode context control registers:
 *
 *   K1 S-mode context base  = 0x201000
 *   K1 per-hart stride      = 0x002000
 *
 *   Each S-mode hart context has:
 *     +0x0: priority threshold
 *     +0x4: claim/complete
 *
 *   hart 0:
 *     base + 0x201000: priority threshold
 *     base + 0x201004: claim/complete
 *
 *   hart 1:
 *     base + 0x203000: priority threshold
 *     base + 0x203004: claim/complete
 *
 *   hart 2:
 *     base + 0x205000: priority threshold
 *     base + 0x205004: claim/complete
 *
 *   ...
 *
 *   hart 7:
 *     base + 0x20F000: priority threshold
 *     base + 0x20F004: claim/complete
 *
 * Address formulas used by the S-mode kernel:
 *
 *   priority(source):
 *     base + source * 4
 *
 *   pending(source):
 *     base + 0x1000 + (source / 32) * 4
 *
 *   enable(hart, source):
 *     base + 0x2080
 *          + hart * 0x100
 *          + (source / 32) * 4
 *
 *   threshold(hart):
 *     base + 0x201000
 *          + hart * 0x2000
 *
 *   claim(hart):
 *     base + 0x201004
 *          + hart * 0x2000
 *
 * According to the RISC-V specification, the PLIC
 * will take up 64MB of physical address space.
 * Most of that address space is spacing/reserved holes 
 * required by the PLIC register layout.
 */

/* 
 * Width of memory mapped PLIC registers is 32-bits
 */

#define PLIC_BASE             		 0xE0000000UL
#define PLIC_MAX_SOURCE_ID     		 159
#define PLIC_CONTEXTS_PER_HART 		 2 
#define PLIC_MAX_PRIORITY      		 7

#define PLIC_MAX_TARGETS       		(PLIC_CONTEXTS_PER_HART * NCPU * PLIC_MAX_SOURCE_ID)

/*
 * Each interrupt source has a priority register associated with it.
 */
#define PRIORITY_BASE                   0
#define PRIORITY_PER_ID                 4

/*
 * Each hart context has a vector of interrupt enable bits associated with it.
 * There's one bit for each interrupt source.
 */
#define PENDING_BASE                    0x1000
#define CONTEXT_ENABLE_BASE             0x2080
#define CONTEXT_ENABLE_SIZE             0x100


/*
 * Each hart context has a set of control registers associated with it.  Right
 * now there's only two: a source priority threshold over which the hart will
 * take an interrupt, and a register to claim interrupts.
 */
#define CONTEXT_BASE                    0x201000
#define CONTEXT_SIZE                    0x2000

#define CONTEXT_THRESHOLD               0x00
#define CONTEXT_CLAIM                   0x04

#define PLIC_DISABLE_THRESHOLD          0x7
#define PLIC_ENABLE_THRESHOLD           0

// #define PLIC_QUIRK_EDGE_INTERRUPT       0

/*
 * Helper macro functions for mmio ops
 */
#define PLIC_PRIORITY(irq)              ((PLIC_BASE + PRIORITY_BASE) + irq * 4)
#define PLIC_PENDING         		(PLIC_BASE + PENDING_BASE)
#define PLIC_SENABLE(hart, bank)   	(PLIC_BASE + CONTEXT_ENABLE_BASE + (hart) * CONTEXT_ENABLE_SIZE + (bank) * 4)
#define PLIC_SPRIORITY(hart) 		(PLIC_BASE + CONTEXT_BASE + (hart) * CONTEXT_SIZE)
#define PLIC_SCLAIM(hart)    		(PLIC_BASE + (CONTEXT_BASE + CONTEXT_CLAIM) + (hart) * CONTEXT_SIZE)

void plicinit(void);
void plicinithart(void);
int plic_claim(void);
void plic_complete(int irq);


#endif
