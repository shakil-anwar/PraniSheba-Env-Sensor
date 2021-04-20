#ifndef _TIMERS_H_
#define _TIMERS_H_
#include <Arduino.h>

#if defined(PROD_BUILD) 
    #include "./src/lib/nRF24/nRF24_Query.h"
#else
    #include "nRF24_Query.h"
#endif 

void TimersBegin();
uint32_t second();

void rtcBegin();
uint32_t rtcGetSec();
void rtcUpdateSec(uint32_t unix);


uint32_t calcNextSlotUnix(uint32_t uSec, nrfNodeConfig_t *conf);
void setNextSlotSec(uint32_t unix);
bool *isTimeTosend();

extern volatile bool _readyToSend;
extern volatile uint32_t _nextSlotSec;

#endif
