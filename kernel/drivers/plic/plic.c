#include <mmio.h>
#include <cpu/cpu.h>
#include <xv6/types.h>
#include <xv6/param.h>
#include <proc/proc.h>
#include <drivers/plic/plic.h>
#include <arch/riscv/riscv.h>
#include <arch/riscv/rv2/irq.h>

/**
 * The RISCV Platform Level Interrupt Controller (PLIC)
 * for the RV2 tailored for xv6.
 */

/**
  * Private list of devices we want to use, the physical hardware
  * interrupt id is directly used (one-to-one), and wont be mapped using a irq
  * number space like in linux. 0 is RESERVED as per the RISC-V specification.
  */
static const uint32 hwirq_ids[] = {IRQ_UART0};
static const uint32 num_dev = (sizeof(hwirq_ids) / sizeof(hwirq_ids[0]));

/**
 * The interrupt-enable registers are split into 5 banks for each hart
 */
#define NUM_BANKS 5

/**
 * For every device in the hwirq_ids array that has a valid hwirq id,
 * we set the priority to 1.
 */
void
plicinit()
{
  // set desired IRQ priorities non-zero (otherwise disabled).
  for (uint32 i = 0; i < num_dev; i++) {
    uint32 irq = hwirq_ids[i];
    if (irq > 0 && irq <= PLIC_MAX_SOURCE_ID) {
      writel(1,(volatile void *)PLIC_PRIORITY(irq));
    } else {
      // printk("plicinit() -> Invalid hwirq id: %d\n", irq);  
      continue;
    }
  }
}

/** 
 * Set enable bits for this hart's S-mode
 * for hardware devices specified in hwirq_ids.
 *
 * Each hart exclusively initializes its own S-mode enable
 * registers during startup. Interrupt routing is not modified
 * afterward, so no lock is required here.
 *
 * If for any reason, toggling bits is required use a lock!!!
 */
void
plicinithart()
{
  int hart = cpuid();

  // Disable each PLIC interrupt source using the KY-specific sequence.
  for (uint32 irq = 1; irq <= PLIC_MAX_SOURCE_ID; irq++) {
    uint32 bank = irq / 32;
    uint32 bit = 1U << (irq & 31);

    volatile void *reg = (volatile void *)PLIC_SENABLE(hart, bank);

    // KY-specific quirk: complete before disabling.
    writel(irq, (volatile void *)PLIC_SCLAIM(hart));

    uint32 val = readl(reg);
    writel(val & ~bit, reg);

    // KY-specific quirk: clear pending, which maybe triggered by uboot
    writel(0, (volatile void*)(PLIC_PENDING + 4 * bank));

  }

  // Enable the interrupts used by xv6.
  for (uint32 i = 0; i < num_dev; i++) {
    uint32 irq = hwirq_ids[i];

    if (irq == 0 || irq > PLIC_MAX_SOURCE_ID)
      continue;

    uint32 bank = irq / 32;
    uint32 bit = 1U << (irq & 31);

    volatile void *reg = (volatile void *)PLIC_SENABLE(hart, bank);

    uint32 val = readl(reg);
    writel(val | bit, reg);

    // KY-specific quirk: complete after enabling.
    writel(irq, (volatile void *)PLIC_SCLAIM(hart));
  }

  // Accept interrupts with priority greater than zero.
  writel(PLIC_ENABLE_THRESHOLD,
         (volatile void *)PLIC_SPRIORITY(hart));
}

/**
 * Ask the PLIC what interrupt we should serve.
 */
int
plic_claim()
{
  int hart = cpuid();
  int irq = readl((volatile void *)PLIC_SCLAIM(hart));
  return irq;
}

/**
 * Tell the PLIC we've served this IRQ.
 */
void
plic_complete(int irq)
{
  int hart = cpuid();
  writel(irq, (volatile void *)PLIC_SCLAIM(hart));
}
