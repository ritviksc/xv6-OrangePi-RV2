#ifndef UART_H
#define UART_H

void            uartinit(void);
void            uartintr(void);
void            uartwrite(char [], int);
void            uartputc_sync(int);


#endif
