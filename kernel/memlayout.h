// Layout NOT FINAL - will need to replace qemu virtual disk
// Using physical memory layout for the OrangePi RV2
// This ensures that xv6 can run properly on the OrangePi RV2

// Using the .dts file for the OrangePi RV2

// E4000000 -- CLINT
// E0000000 -- PLIC
// d4017000 -- UART0
// 10001000 -- virtio disk
// 80000000 -- qemu's boot ROM loads the kernel here,
//             then jumps here.
// unused RAM after 80000000.

// the kernel uses physical memory thus:
// 0x11000000 -- entry.S, then kernel text and data
// end -- start of kernel page allocation area
// PHYSTOP -- end RAM used by the kernel

#define DEVICE_NAME "OrangePi RV2" 

#define TIMEBASE_FREQ 24000000L  // 24MHz
#define TIMER_HZ 10 // timer interrupts per second (~100ms scheduling granularity like QEMU xv6)
#define TIMEBASE_FREQ/TIMER_HZ // clock ticks between one timer interrupt and the next

#define UART0     0xd4017000L
#define UART0_IRQ 42

// virtio mmio interface
// #define VIRTIO0     0x10001000
// #define VIRTIO0_IRQ 1

#define PLIC                 0xE0000000L
#define PLIC_PRIORITY        (PLIC + 0x0)
#define PLIC_PENDING         (PLIC + 0x1000)
#define PLIC_SENABLE(hart)   (PLIC + 0x2080 + (hart) * 0x100)
#define PLIC_SPRIORITY(hart) (PLIC + 0x201000 + (hart) * 0x2000)
#define PLIC_SCLAIM(hart)    (PLIC + 0x201004 + (hart) * 0x2000)

// the kernel expects there to be RAM
// for use by the kernel and user pages
// from physical address 0x11000000 to PHYSTOP.
#define KERNBASE 0x11000000L

// For inital port let system only have 128MB of free RAM
#define PHYSTOP  (KERNBASE + 128 * 1024 * 1024)

// map the trampoline page to the highest address,
// in both user and kernel space.
#define TRAMPOLINE (MAXVA - PGSIZE)

// map kernel stacks beneath the trampoline,
// each surrounded by invalid guard pages.
#define KSTACK(p) (TRAMPOLINE - ((p) + 1) * 2 * PGSIZE)

// User memory layout.
// Address zero first:
//   text
//   original data and bss
//   fixed-size stack
//   expandable heap
//   ...
//   TRAPFRAME (p->trapframe, used by the trampoline)
//   TRAMPOLINE (the same page as in the kernel)
#define TRAPFRAME (TRAMPOLINE - PGSIZE)
