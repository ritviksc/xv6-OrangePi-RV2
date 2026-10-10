#ifndef PRINTK_H
#define PRINTK_H

int             printk(char*, ...) __printf(1,2);
void            panic(char*) __noreturn;
void            printkinit(void);

#endif
