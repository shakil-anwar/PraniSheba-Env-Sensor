#ifndef _WATCHDOG_H_
#define _WATCHDOG_H_

#include <Arduino.h>


typedef void (*wdtFun_t)(void);

void wdtEnable(uint16_t timeMs);
void wdtAttach(wdtFun_t cb);
void wdtStart();

void wdtBegin(uint16_t timeMs, wdtFun_t wdtCb);
void wdtReset();
void wdtDisable();

// #define wdtReset() wdt_reset()


#endif //End _WATCHDOG_H_
