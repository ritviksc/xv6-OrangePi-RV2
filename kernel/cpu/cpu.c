#include <cpu/cpu.h>
#include <xv6/types.h>
#include <xv6/param.h>
#include <arch/riscv/riscv.h>

struct cpu cpus[NCPU];

int
cpuid()
{
  int id = r_tp();
  return id;
}

// Return this CPU's cpu struct.
// Interrupts must be disabled.
struct cpu *
mycpu(void)
{
  int id = cpuid();
  struct cpu *c = &cpus[id];
  return c;
}


