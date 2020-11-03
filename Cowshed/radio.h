#ifndef _RADIO_H_
#define _RADIO_H_
#include "Arduino.h"
#include <SPI.h>
#include <nRF24L01.h>
#include <nRF24.h>
#include "led.h"
#include "MemQ.h"
#include "time.h"


void radio_begin(void);
void readPayload();
extern MemQ memQ;

extern bool rf_send_success;
#endif
