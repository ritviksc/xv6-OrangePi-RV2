#ifndef BARRIER_H
#define BARRIER_H

#define RISCV_NOP \
        __asm__ __volatile__ ("nop")

#define RISCV_NOP_BARRIER \
	__asm__ __volatile__ ("nop" : : : "memory")

#define COMPILER_BARRIER \
	__asm__ __volatile__ ("" : : : "memory")

#define RISCV_FENCE(p, s) \
        __asm__ __volatile__ ("fence " #p "," #s : : : "memory")

#endif
