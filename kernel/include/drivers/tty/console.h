#ifndef CONSOLE_H
#define CONSOLE_H

#include <xv6/types.h>

void            consoleinit(void);
// void            consoleintr(int);
void            consputc(int);
int		consolewrite(int user_src, uint64 src, int n);



#endif
