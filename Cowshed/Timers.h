#ifndef _TIMERS_H_
#define _TIMERS_H_
#include <Arduino.h>

void TimersBegin();
uint32_t second();

void rtcBegin();
uint32_t rtcGetSec();
void rtcUpdateSec(uint32_t unix);

#endif
