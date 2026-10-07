#ifndef TRAP_H
#define TRAP_H

#include <xv6/types.h>

// trap.c
void            trapinit(void);
void            trapinithart(void);
uint64 		usertrap();
void 		prepare_return();
void		kerneltrap();
void		clockintr();
int		devintr();

#endif
