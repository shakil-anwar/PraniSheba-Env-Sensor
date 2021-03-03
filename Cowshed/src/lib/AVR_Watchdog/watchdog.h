#ifndef _WATCHDOG_H_
#define _WATCHDOG_H_

#include <Arduino.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>

typedef void (*func_t)(void);

void wdtEnable(uint16_t timeMs);
void wdtAttach(func_t cb);
void wdtStart();
#define wdtReset() wdt_reset()


#endif //End _WATCHDOG_H_
