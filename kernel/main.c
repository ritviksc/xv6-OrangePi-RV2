#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"

volatile static int started = 0;

// start() jumps here on all CPUs eventually.
void
main()
{
  if (cpuid() == 0) {
    consoleinit();
    printkinit();
    printk("\n");
    printk("DEVICE:%s\n",DEVICE_NAME);
    printk("xv6 kernel is booting\n");
    printk("\n");

    // Check if any harts are not starting
    // and report them and continue as normal.
    // If required to be implemented, let user decide to continue using device
    // or quit.
    for (int i = 1; i < NCPU; i++) {
      if (hart_start_failed[i]) {
        printk("hart %d failed to start\n", i);
      }
    }

    kinit();            // physical page allocator
    kvminit();          // create kernel page table
    kvminithart();      // turn on paging
    procinit();         // process table
    trapinit();         // trap vectors
    trapinithart();     // install kernel trap vector
    plicinit();         // set up interrupt controller
    plicinithart();     // ask PLIC for device interrupts
    binit();            // buffer cache
    iinit();            // inode table
    fileinit();         // file table
    virtio_disk_init(); // emulated hard disk - will need to replace this
    userinit();         // first user process
    __atomic_thread_fence(__ATOMIC_SEQ_CST);
    started = 1;
  } else {
    while (started == 0)
      ;
    __atomic_thread_fence(__ATOMIC_SEQ_CST);

    printk("hart %d starting\n", cpuid());
    kvminithart();  // turn on paging
    trapinithart(); // install kernel trap vector
    plicinithart(); // ask PLIC for device interrupts
  }

  scheduler();
}
