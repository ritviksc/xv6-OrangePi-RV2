#include <cpu/cpu.h>
#include <xv6/types.h>
#include <xv6/param.h>
#include <trap/trap.h>
#include <arch/riscv/rv2/irq.h>
#include <arch/riscv/rv2/memlayout.h>
#include <arch/riscv/riscv.h>
#include <drivers/uart/uart.h>
#include <drivers/plic/plic.h>
#include <drivers/tty/printk.h>
#include <locks/spinlock.h>
#include <proc/proc.h>

struct spinlock tickslock;
uint ticks;

extern uint64 timer;
extern char trampoline[], uservec[];

// kernelvec.S calls kerneltrap().
extern void kernelvec();
extern int devintr();

void
trapinit()
{
  initlock(&tickslock, "time");
}

/**
 * Set up to take exceptions and traps while in the kernel.
 */
void
trapinithart(void)
{
  w_stvec((uint64)kernelvec);
}

/**
 * Handle an interrupt, exception, or system call from user space.
 * called from, and returns to, trampoline.S
 * @return user satp for trampoline.S to switch to.
 */


/* uint64
usertrap()
{
  int which_dev = 0;

  if ((r_sstatus() & SSTATUS_SPP) != 0)
    panic("usertrap() -> not from user mode");

  **
   * Send interrupts and exceptions to kerneltrap(),
   * since we're now in the kernel.
   *
  w_stvec((uint64)kernelvec);

  struct proc *p = myproc();

  // save user program counter.
  p->trapframe->epc = r_sepc();

  if (r_scause() == 8) {
    // system call

    if (killed(p))
      kexit(-1);
    ** 
     * Sepc points to the ecall instruction,
     * but we want to return to the next instruction.
     
    p->trapframe->epc += 4;

    ** 
     * An interrupt will change sepc, scause, and sstatus,
     * so enable only now that we're done with those registers.
     *
    intr_on();

    syscall();
  } else if ((which_dev = devintr()) != 0) {
    // ok
  } else if ((r_scause() == 15 || r_scause() == 13) &&
             vmfault(p->pagetable, r_stval(), (r_scause() == 13) ? 1 : 0) !=
               0) {
    // page fault on lazily-allocated page
  } else {
    printk("usertrap() -> unexpected scause 0x%lx pid=%d\n", r_scause(), p->pid);
    printk("              sepc=0x%lx stval=0x%lx\n", r_sepc(), r_stval());
    setkilled(p);
  }

  if (killed(p))
    kexit(-1);

  // give up the CPU if this is a timer interrupt.
  if (which_dev == 2)
    yield();

  prepare_return();

  // the user page table to switch to, for trampoline.S
  uint64 satp = MAKE_SATP(p->pagetable);

  // return to trampoline.S; satp value in a0.
  return satp;
}
*/

/**
 * Set up trapframe and control registers for a return to user space
 *
void
prepare_return()
{
  struct proc *p = myproc();

  ** 
   * We're about to switch the destination of traps from
   * kerneltrap() to usertrap(). because a trap from kernel
   * code to usertrap would be a disaster, turn off interrupts.
   *

  intr_off();

  // send syscalls, interrupts, and exceptions to uservec in trampoline.S
  uint64 trampoline_uservec = TRAMPOLINE + (uservec - trampoline);
  w_stvec(trampoline_uservec);

  // set up trapframe values that uservec will need when
  // the process next traps into the kernel.
  p->trapframe->kernel_satp = r_satp();         // kernel page table
  p->trapframe->kernel_sp = p->kstack + PGSIZE; // process's kernel stack
  p->trapframe->kernel_trap = (uint64)usertrap;
  p->trapframe->kernel_hartid = r_tp(); // hartid for cpuid()

  // set up the registers that trampoline.S's sret will use
  // to get to user space.

  // set S Previous Privilege mode to User.
  unsigned long x = r_sstatus();
  x &= ~SSTATUS_SPP; // clear SPP to 0 for user mode
  x |= SSTATUS_SPIE; // enable interrupts in user mode
  w_sstatus(x);

  // set S Exception Program Counter to the saved user pc.
  w_sepc(p->trapframe->epc);
}
*/

/**
 * Interrupts and exceptions that occur from executing kernel code
 * are handled by this trap handler, on whatever the current kernel stack is.
 */
void
kerneltrap()
{
  int which_dev = 0;
  uint64 sepc = r_sepc();
  uint64 sstatus = r_sstatus();
  uint64 scause = r_scause();

  if ((sstatus & SSTATUS_SPP) == 0)
    panic("kerneltrap() -> not from supervisor mode");
  
  if (intr_get() != 0)
    panic("kerneltrap() -> interrupts enabled");

  if ((which_dev = devintr()) == 0) {
    // interrupt or trap from an unknown source
    printk("kerneltrap() -> scause=0x%lx sepc=0x%lx stval=0x%lx\n", scause, r_sepc(),
           r_stval());
    panic("kerneltrap() -> unknown trap source");
  }

  // give up the CPU if this is a timer interrupt.
  if (which_dev == 2){ // && myproc() != 0)
    // yield();
    if ((ticks & 1023) == 0)
      printk("kerneltrap() -> timer interrupt\n");
  }

  // the yield() may have caused some traps to occur,
  // so restore trap registers for use by kernelvec.S's sepc instruction.
  w_sepc(sepc);
  w_sstatus(sstatus);
}

void
clockintr()
{
  // kernels notion of time
  if (cpuid() == 0) {
    acquire(&tickslock);
    ticks++;
   //  wakeup(&ticks);
    release(&tickslock);
  }

  /**
   * Ask for the next timer interrupt. 
   * This also clears the interrupt request.
   * TIMER_TICKS is about a tenth
   * of a second w.r.t to RV2 hart frequency.
   */
  timer += TIMER_TICKS;
  w_stimecmp(timer);
}

/** 
 * Check if it's an external interrupt or software interrupt,
 * and handle it.
 * In the scause register, the MSB will either 0 or 1.
 * If it is 0, an synchronous interrupt occured, else if its 1
 * an asynchronous interrupt occured. xv6 as of now supports
 * supervisor external and timer interrupts.
 * @return 2 if timer interrupt
 * 	   1 if other device
 * 	   0 if not recognized
 */
int
devintr()
{
  uint64 scause = r_scause();

  // 1000...1001 - least 4 bits represent 9
  if (scause == 0x8000000000000009L) {
    // this is a supervisor external interrupt, via PLIC.

    printk("External interrupt\n");
    // irq indicates which device interrupted.
    int irq = plic_claim();
    printk("External irq id: %d\n", irq);

    if (irq == IRQ_UART0) {
      uartintr();
    } else if (irq) {
      printk("devintr() -> unexpected interrupt irq=%d\n", irq);
    }

    /** 
     * The PLIC allows each device to raise at most one
     * interrupt at a time; tell the PLIC the device is
     * now allowed to interrupt again. This is necessary so
     * device wont be "pending" potentially 
     * causing an interrupt storm.
     */
    if (irq)
      plic_complete(irq);

    return 1;

  // 1000...0101 - least 4 bits represent 5
  } else if (scause == 0x8000000000000005L) {
      // timer interrupt.
      clockintr();
      return 2;
  } else {
      return 0;
  }
}
