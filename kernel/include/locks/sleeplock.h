#ifndef SLEEPLOCK_H
#define SLEEPLOCK_H

#include <xv6/types.h>

// Long-term locks for processes
struct sleeplock {
  uint8 locked;        // Is the lock held?
  struct spinlock lk; // spinlock protecting this sleep lock

  // For debugging:
  char *name; // Name of lock.
  int pid;    // Process holding lock
};

#endif
