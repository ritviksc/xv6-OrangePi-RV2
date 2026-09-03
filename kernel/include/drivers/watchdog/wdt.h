#ifndef WDT_H
#define WDT_H

#include <xv6/types.h>

void wdt_start(uint64);
void wdt_reset();
void wdt_stop();
void wdt_expire_now();

#endif
