#ifndef SPINLOCK_H
#define SPINLOCK_H

#include <xv6/types.h>

/**
 * A spinlock is a lock which causes a thread/process trying to accquire it
 * simply wait (spin) in a loop, while checking continously if the lock is 
 * avaliable, so it can use the resource next.
 * This consumes CPU cycles, which are precious, so should be used
 * when the critical section is short.
 */
struct spinlock {
  uint locked; // Is the lock held?

  // For debugging:
  char *name;      // Name of lock.
  struct cpu *cpu; // The cpu holding the lock.
};

void initlock(struct spinlock *lk, char *name);
void acquire(struct spinlock *lk);
void release(struct spinlock *lk);
int holding(struct spinlock *lk);
void push_off(void);
void pop_off(void);


#endif
