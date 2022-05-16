#ifndef _TIMERS_H_
#define _TIMERS_H_
#include <Arduino.h>
#include "IoT.h"

#if defined(PROD_BUILD) 
    #include "./lib/nRF24/nRF24_Client.h"
#else
    #include "nRF24_Client.h"
#endif 

#if defined(ESP32_V010)
    #include "utility.h"
    uint32_t getNtpTime();
#endif

void TimersBegin();
uint32_t second();

void rtcBegin();
uint32_t rtcGetSec();
void rtcUpdateSec(uint32_t unix);


uint32_t calcNextSlotUnix(uint32_t uSec, nrfNodeConfig_t *conf);
void setNextSlotSec(uint32_t unix);
bool *isTimeTosend();
bool rtcIsRunning();
void printSecond();
// #if defined(ESP32_V010)
// void printSecond();
// #endif
extern volatile bool _readyToSend;
extern volatile uint32_t _nextSlotSec;

// #if defined(ESP32_V010) && !defined(SOFTWARE_I2C)
// extern ErriezDS1307 RTC;
// #endif

#endif
