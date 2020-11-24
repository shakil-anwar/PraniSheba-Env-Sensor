#ifndef _RADIO_H_
#define _RADIO_H_
#include <Arduino.h>
#include "nRF24.h"
#include "nRF24_Query.h"
#include "led.h"
#include "MemQ.h"



void radio_begin(void);
void radioStart();
uint32_t getRtcTime();
extern MemQ memQ;

extern bool rf_send_success;
extern int retryCount;
#endif
