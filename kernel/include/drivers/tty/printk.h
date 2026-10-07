#ifndef PRINTK_H
#define PRINTK_H

int             printk(char*, ...) __attribute__ ((format (printf, 1, 2)));
void            panic(char*) __attribute__((noreturn));
void            printkinit(void);

#endif
